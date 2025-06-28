#include <array>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <bit>
#include <limits>

template <typename T>
concept is_inheritable =
    !std::is_final_v<T> && !std::is_enum_v<T> && !std::is_fundamental_v<T> &&
    !std::is_union_v<T> && !std::is_function_v<T> && std::is_class_v<T>;

// NOTE: 不必追求标准布局类型 reinterpret_cast + offset 仅仅
// NOTE: C接口再使用吧。c结构体都是标准布局，才保证不会有问题
template <std::default_initializable T>
    requires(is_inheritable<T>)
struct object_pool
{
    // T chunk[block_count];
    struct chunk;
    struct double_link
    { // base class for intrusive doubly-linked structures
        double_link *next;
        double_link *prev;
    };
    // NOTE: chunk 要放在双向链表中
    struct link_item : double_link
    {
        chunk item; // NOLINT
        // NOTE: constexpr 是不能 有 this 的
        link_item() noexcept : item(this) {}
    };
    struct object_storage_type : public T
    {
        chunk *chunk_ptr{};
    };
    struct chunk
    {
        using status_type = std::size_t;
        static constexpr auto block_count = sizeof(status_type) * CHAR_BIT; // NOLINT

        explicit chunk(link_item *parent_ptr) noexcept
            : parent_(parent_ptr), status_{~status_type{0}}
        {
            assert(status_ == std::numeric_limits<status_type>::max());
            for (std::size_t i{0}; i < block_count; ++i)
            {
                buffer_[i].chunk_ptr = this; // NOLINT
            }
        }

        [[nodiscard]] constexpr auto free_count() const noexcept // NOLINT
        {
            return std::popcount(status_);
        }

        constexpr T *allocate() noexcept
        {
            return empty() ? nullptr : do_allocate();
        }

        constexpr T *do_allocate() noexcept // NOLINT
        {
            assert(status_ != 0);

            const auto k_index = std::countr_zero(status_);
            std::cout << "  do_allocate: index " << k_index << "\n";
            return (set_zero(k_index), static_cast<T *>(&buffer_[k_index])); // NOLINT
        }
        constexpr void deallocate(T *ptr) noexcept
        {
            set_one(as_storage(ptr) - &buffer_[0]);
        }
        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return status_ == 0;
        }

        static constexpr object_storage_type *as_storage(T *ptr) noexcept // NOLINT
        {
            return static_cast<object_storage_type *>(ptr);
        }

        [[nodiscard]] constexpr auto *parent() const noexcept
        {
            return parent_;
        }

      private: // 添加父对象指针//NOTE: 地址偏移量只能运行时，且标准布局才安全
        link_item *parent_;
        status_type status_;
        object_storage_type buffer_[block_count]; // NOLINT

        constexpr void set_one(std::uint8_t index) noexcept // NOLINT
        {
            // NOTE: status_type{1} 是必须的。否则bit位截断，溢出未定义
            status_ |= (status_type{1} << index);
        }
        constexpr void set_zero(std::uint8_t index) noexcept // NOLINT
        {
            status_ &= ~(status_type{1} << index);
        }
    };

    // NOTE: 核心还是 为了避免零碎稀松占用 chunk.一起释放和申请？
    // NOTE: 需要 map 来讲 容量 + 同容量的 link_item 关联起来
    static constexpr auto pool_size = chunk::block_count + 1; // NOLINT

    explicit object_pool(std::size_t init_count) // NOTE: 有 new 关键肯定 constexpr 不了
    {
        for (std::size_t i = 0; i < init_count; i += chunk::block_count)
        {
            pool_[chunk::block_count].push_back(new link_item());
            ++link_item_count_;
        }
    }
    constexpr ~object_pool() noexcept
    {
        for (std::size_t i{0}; i < pool_size; ++i)
        {
            intrusive_list<link_item> &free_list = pool_[i]; // NOLINT
            while (not free_list.empty())
            {
                link_item *link = free_list.pop_front();
                allocate_count_ -= chunk::block_count - link->item.free_count();
                --link_item_count_;
                delete link;
            }
        }
        // NOTE: 不需要 析构，因为专门为buffer定义的。
        // NOTE: 本来就是仅仅提供 内存地址的。怎么用不关心
        assert(link_item_count_ == 0);
        assert(allocate_count_ == 0);
    }
    object_pool(const object_pool &) = delete;
    object_pool(object_pool &&) = delete;
    object_pool &operator=(const object_pool &) = delete;
    object_pool &operator=(object_pool &&) = delete;

    constexpr auto try_shrink(std::size_t max_shrink_count) noexcept // NOLINT
    {
        intrusive_list<link_item> &free_list = pool_[chunk::block_count];
        std::size_t shrink_count{0};
        while (shrink_count + chunk::block_count <= max_shrink_count &&
               !free_list.empty())
        {
            link_item *item = free_list.pop_back();
            --link_item_count_;
            shrink_count += chunk::block_count;
            delete item;
        }
        return shrink_count;
    }
    constexpr float usage_rate() noexcept // NOLINT
    {
        return (1.0 * allocate_count_) / (link_item_count_ * chunk::block_count);
    }

    constexpr T *allocate()
    {
        // NOTE: 理论上必须能得到
        for (std::size_t i{1}; i < pool_size; ++i)
        {
            if (intrusive_list<link_item> &free_list = pool_[i]; // NOLINT
                not free_list.empty())
            {
                link_item *link = pool_[i].pop_front(); // NOLINT
                std::cout << "i: " << i << ", free_count: " << link->item.free_count()
                          << '\n';
                assert(link->item.free_count() == i);
                T *ptr = link->item.do_allocate();
                pool_[i - 1].push_back(link); // NOLINT
                ++allocate_count_;
                return ptr;
            }
        }
        // NOTE: need new link_item to allocate T* storage
        auto *link = new link_item();
        T *ptr = link->item.do_allocate();
        pool_[chunk::block_count - 1].push_back(link);
        ++link_item_count_;
        ++allocate_count_;
        return ptr;
    }
    constexpr void deallocate(T *ptr) noexcept
    {
        object_storage_type *storage = chunk::as_storage(ptr);
        chunk *c_ptr = storage->chunk_ptr;
        // NOTE: offsetof 要求 link_item 必须是：standard-layout
        // constexpr std::size_t k_offset = offsetof(link_item, item);
        // 直接通过父指针获取 link_item
        link_item *item = c_ptr->parent();

        const std::size_t k_old_free_count = c_ptr->free_count();
        c_ptr->deallocate(ptr);
        assert(k_old_free_count < chunk::block_count);

        pool_[k_old_free_count].remove(item);        // NOLINT
        pool_[k_old_free_count + 1].push_back(item); // NOLINT

        --allocate_count_;
    }

  private:
    template <typename Ty>
        requires(std::derived_from<Ty, double_link>)
    struct intrusive_list
    { // intrusive circular list of Ty, which must derive from _Double_link<_Tag>
        using link_type = double_link;
        constexpr intrusive_list() noexcept = default;
        intrusive_list(intrusive_list &&) = delete;
        intrusive_list &operator=(intrusive_list &&) = delete;
        intrusive_list(const intrusive_list &) = delete;
        intrusive_list &operator=(const intrusive_list &) = delete;
        ~intrusive_list() noexcept = default;

        // target: head_ -> item -> head_->next
        constexpr void push_front(Ty *const item) noexcept // NOLINT
        { // insert item at the head of this list
            insert_before(head_.next, static_cast<link_type *>(item));
        }

        // target:  head_->prev -> item -> head_
        constexpr void push_back(Ty *const item) noexcept // NOLINT
        { // insert item at the end of this list
            insert_before(&head_, static_cast<link_type *>(item));
        }

        [[nodiscard]] constexpr Ty *pop_front() noexcept // NOLINT
        {
            if (empty())
                return nullptr;
            auto *node = head_.next;
            remove(as_item(node));
            return as_item(node);
        }
        [[nodiscard]] constexpr Ty *pop_back() noexcept // NOLINT
        {
            if (empty())
                return nullptr;
            auto *node = head_.prev;
            remove(as_item(node));
            return as_item(node);
        }

        static constexpr void remove(Ty *const item) noexcept // NOLINT
        { // unlink item from this list
            auto *ptr = static_cast<link_type *>(item);
            ptr->next->prev = ptr->prev;
            ptr->prev->next = ptr->next;
        }

        constexpr void clear() noexcept
        { // make this list empty
            head_.next = &head_;
            head_.prev = &head_;
        }

        [[nodiscard]] constexpr bool empty() const noexcept
        {
            return head_.next == &head_ && head_.prev == &head_;
        }

      private:
        link_type head_{&head_, &head_};

        // NOLINTNEXTLINE
        static constexpr void insert_before(link_type *pos, link_type *item) noexcept
        {
            item->next = pos;
            item->prev = pos->prev;
            // NOTE: first update  pos->prev->next then pos->prev
            pos->prev->next = item;
            pos->prev = item;
        }
        static constexpr link_type *as_link(Ty *const ptr) noexcept // NOLINT
        {
            // extract the link from the item denoted by _Ptr
            return static_cast<link_type *>(ptr);
        }

        static constexpr Ty *as_item(link_type *const ptr) noexcept // NOLINT
        { // get the item whose link is denoted by _Ptr
            return static_cast<Ty *>(ptr);
        }
    };

    std::size_t allocate_count_{0};             // NOLINT
    std::size_t link_item_count_{0};            // NOLINT
    intrusive_list<link_item> pool_[pool_size]; // NOLINT
};

#include <cassert>
#include <vector>
#include <iostream>

// NOLINTBEGIN

// 测试对象类型
struct test_object
{
    int value;
    char data[32];
    test_object(int v = 0) : value(v) {}
};

// 测试对象池
template <bool constexpr_check = false>
constexpr void test_object_pool()
{
    // 基本分配/释放测试
    {
        object_pool<test_object> pool(1);

        // 分配对象
        test_object *obj1 = pool.allocate();
        obj1->value = 42;
        assert(obj1->value == 42);

        // 释放对象
        pool.deallocate(obj1);

        // 重用内存
        test_object *obj2 = pool.allocate();
        assert(obj2 == obj1);      // 应重用相同内存
        assert(obj2->value == 42); // 内存未被清除（符合预期）
    }

    // 批量分配测试
    {
        constexpr size_t count = 200;
        object_pool<test_object> pool(1);
        std::vector<test_object *> objects;

        // 分配大量对象
        for (int i = 0; i < count; ++i)
        {
            auto *obj = pool.allocate();
            obj->value = i;
            objects.push_back(obj);
        }

        // 验证对象值
        for (int i = 0; i < count; ++i)
        {
            assert(objects[i]->value == i);
        }

        // 释放所有对象
        for (auto *obj : objects)
        {
            pool.deallocate(obj);
        }
    }

    // 内存回收测试
    {
        object_pool<test_object> pool(1);
        const size_t block_count = decltype(pool)::chunk::block_count;

        // 分配对象填满第一个chunk
        std::vector<test_object *> first_chunk;
        for (size_t i = 0; i < block_count; ++i)
        {
            first_chunk.push_back(pool.allocate());
        }

        // 分配额外对象（应创建新chunk）
        test_object *extra_obj = pool.allocate();

        // 释放第一个chunk的所有对象
        for (auto *obj : first_chunk)
        {
            pool.deallocate(obj);
        }

        // 收缩内存池（应能回收第一个chunk）
        const size_t shrunk = pool.try_shrink(block_count);
        assert(shrunk == block_count);

        // 分配新对象（应使用新chunk）
        test_object *new_obj = pool.allocate();
        assert(new_obj != first_chunk[0]); // 内存应来自新chunk
    }

    // 使用率计算测试
    {
        object_pool<test_object> pool(1);
        const size_t block_count = decltype(pool)::chunk::block_count;

        // 初始使用率应为0
        assert(pool.usage_rate() == 0.0f);

        // 分配部分对象
        const size_t allocate_count = block_count / 2;
        std::vector<test_object *> objects;
        for (size_t i = 0; i < allocate_count; ++i)
        {
            objects.push_back(pool.allocate());
        }

        // 检查使用率
        const float expected_rate = static_cast<float>(allocate_count) / block_count;
        assert(pool.usage_rate() == expected_rate);

        // 释放一半对象
        for (size_t i = 0; i < allocate_count / 2; ++i)
        {
            pool.deallocate(objects[i]);
        }

        // 检查更新后的使用率
        const float updated_rate = static_cast<float>(allocate_count / 2) / block_count;
        assert(pool.usage_rate() == updated_rate);
    }

    // 边界条件测试
    {
        object_pool<test_object> pool(0); // 初始容量为0

        // 首次分配应成功
        test_object *obj = pool.allocate();
        assert(obj != nullptr);

        // 释放后立即重新分配
        pool.deallocate(obj);
        test_object *new_obj = pool.allocate();
        assert(new_obj == obj);

        // 尝试收缩空池
        assert(pool.try_shrink(100) == 0);
    }

    // 对象池扩展测试
    {
        object_pool<test_object> pool(1);
        const size_t block_count = decltype(pool)::chunk::block_count;

        // 分配超过初始容量的对象
        std::vector<test_object *> objects;
        for (size_t i = 0; i < block_count * 3; ++i)
        {
            objects.push_back(pool.allocate());
        }

        // 验证所有分配成功
        for (auto *obj : objects)
        {
            assert(obj != nullptr);
        }
    }

    // 内存位置验证测试
    {
        object_pool<test_object> pool(1);

        // 分配两个对象
        test_object *obj1 = pool.allocate();
        test_object *obj2 = pool.allocate();

        // 计算内存距离
        if constexpr (not constexpr_check)
        {
            size_t distance =
                reinterpret_cast<char *>(obj2) - reinterpret_cast<char *>(obj1);
            size_t expected = sizeof(object_pool<test_object>::object_storage_type);
            assert(distance == expected);
        }
    }
    if constexpr (not constexpr_check)
    {
        std::cout << "All object_pool tests passed!\n";
    }
}

struct destruct_tracker
{
    static int destruct_count;
    ~destruct_tracker()
    {
        ++destruct_count;
    }
};
int destruct_tracker::destruct_count = 0;

void test_destructor_calls()
{
    {
        object_pool<destruct_tracker> pool(1);
        auto *obj = pool.allocate(); // 触发chunk创建
        // 不显式调用析构（依赖池销毁）
    } // 池析构时应销毁chunk并调用所有对象的析构
    assert(destruct_tracker::destruct_count ==
           object_pool<destruct_tracker>::chunk::block_count);
}

struct alignas(64) aligned_type
{
    char data[64];
};

void test_alignment()
{
    object_pool<aligned_type> pool(1);
    std::vector<aligned_type *> objects;
    for (int i = 0; i < 10; ++i)
    {
        auto *ptr = pool.allocate();
        assert(reinterpret_cast<uintptr_t>(ptr) % alignof(aligned_type) == 0);
        objects.push_back(ptr);
    }
    for (auto *ptr : objects)
        pool.deallocate(ptr);
}

void test_shrink_threshold()
{
    struct obj
    {
        char buffer[64];
    };
    object_pool<obj> pool(1);
    const size_t block_count = decltype(pool)::chunk::block_count;

    // 分配并释放一个chunk
    std::vector<obj *> objs;
    for (size_t i = 0; i < block_count; ++i)
    {
        objs.push_back(pool.allocate());
    }
    for (auto *p : objs)
        pool.deallocate(p);

    // 尝试收缩（需求小于完整chunk）
    size_t shrunk = pool.try_shrink(block_count - 1);
    assert(shrunk == 0); // 应不收缩

    // 精确收缩
    shrunk = pool.try_shrink(block_count);
    assert(shrunk == block_count);
}

// NOLINTEND

int main()
{
    test_object_pool();

    // NOTE: 设计有 this. 就不可能 constexpr 通过
    //  constexpr auto res = []() constexpr {
    //      test_object_pool<true>();
    //      return true;
    //  }();
    //  static_assert(res);

    test_destructor_calls();

    test_alignment();

    test_shrink_threshold(); // 需要继承 int ？ //NOTE: 为了 避免 reinterpret_cast
    return 0;
}