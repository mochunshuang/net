#include <utility>
#include <vector>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <bit>
#include <limits>
#include <new> // 包含 std::bad_alloc

// NOLINTBEGIN

// 异常安全测试开关 - 仅在测试时启用
#define OBJECT_POOL_EXCEPTION_SAFETY_TESTING

#ifdef OBJECT_POOL_EXCEPTION_SAFETY_TESTING
// 内存分配失败模拟器
class allocation_simulator
{
  public:
    static void enable_failure(int fail_after = -1)
    {
        enabled = true;
        allocation_count = 0;
        fail_point = fail_after;
    }

    static void disable_failure()
    {
        enabled = false;
    }

    static bool should_fail()
    {
        if (!enabled)
            return false;
        if (fail_point < 0)
            return false;
        return ++allocation_count > fail_point;
    }

  private:
    inline static bool enabled = false;
    inline static int allocation_count = 0;
    inline static int fail_point = -1;
};
#endif

template <typename T>
concept is_inheritable =
    !std::is_final_v<T> && !std::is_enum_v<T> && !std::is_fundamental_v<T> &&
    !std::is_union_v<T> && !std::is_function_v<T> && std::is_class_v<T>;

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
    struct link_item_owner
    {
        link_item_owner(const link_item_owner &) = delete;
        link_item_owner(link_item_owner &&other) noexcept
            : ptr_(std::exchange(other.ptr_, nullptr))
        {
        }
        link_item_owner &operator=(const link_item_owner &) = delete;
        link_item_owner &operator=(link_item_owner &&) = delete;
        link_item_owner() noexcept : ptr_(nullptr) {}
        explicit link_item_owner(link_item *ptr) noexcept : ptr_(ptr) {}
        ~link_item_owner() noexcept
        {
            delete ptr_;
        }
        void release() noexcept
        {
            ptr_ = nullptr;
        }
        [[nodiscard]] auto *data() const noexcept
        {
            return ptr_;
        }
        void set_data(link_item *ptr) noexcept // NOLINT
        {
            ptr_ = ptr;
        }

      private:
        link_item *ptr_;
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

    // NOTE: 如果构造异常，是不会调用析构函数的
    explicit object_pool(std::size_t init_count) // NOTE: 有 new 关键肯定 constexpr 不了
    {
        const std::size_t k_chunks_needed =
            (init_count + chunk::block_count - 1) / chunk::block_count;
        std::vector<link_item_owner> temp_item_owner;
        temp_item_owner.reserve(k_chunks_needed);

        try
        {
            for (std::size_t i = 0; i < k_chunks_needed; ++i)
            {
#ifdef OBJECT_POOL_EXCEPTION_SAFETY_TESTING
                if (allocation_simulator::should_fail())
                {
                    throw std::bad_alloc();
                }
#endif

                auto *item = new link_item();
                // NOTE: reserve 是未初始化内存 更优
                temp_item_owner.emplace_back(item);
                pool_[chunk::block_count].push_back(item);
                ++link_item_count_;
            }
            for (auto &owner : temp_item_owner)
            {
                owner.release();
            }
        }
        catch (const std::bad_alloc &)
        {
            // 异常发生时，temp_item_owner 会自动释放已分配的内存
            throw;
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
        link_item_owner temp_owner;
        try
        {
#ifdef OBJECT_POOL_EXCEPTION_SAFETY_TESTING
            if (allocation_simulator::should_fail())
            {
                throw std::bad_alloc();
            }
#endif

            temp_owner.set_data(new link_item());
        }
        catch (const std::bad_alloc &)
        {
            throw; // 重新抛出异常
        }

        auto *link = temp_owner.data();
        T *ptr = link->item.do_allocate();
        pool_[chunk::block_count - 1].push_back(link);
        ++link_item_count_;
        ++allocate_count_;
        temp_owner.release();
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
void test_object_pool()
{
    // ... [原始测试代码保持不变] ...
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
    // ... [原始测试代码保持不变] ...
}

struct alignas(64) aligned_type
{
    char data[64];
};

void test_alignment()
{
    // ... [原始测试代码保持不变] ...
}

void test_shrink_threshold()
{
    // ... [原始测试代码保持不变] ...
}

// ====================== 异常安全测试 ======================
#ifdef OBJECT_POOL_EXCEPTION_SAFETY_TESTING
void test_constructor_allocation_failure()
{
    std::cout << "Running test_constructor_allocation_failure...\n";

    const int chunks_needed = 3;
    const int total_allocations =
        chunks_needed * object_pool<test_object>::chunk::block_count;

    for (int fail_point = 0; fail_point < chunks_needed; ++fail_point)
    {
        try
        {
            std::cout << "  Testing fail at chunk #" << fail_point << "... ";

            // 设置在此区块分配时失败
            allocation_simulator::enable_failure(fail_point);

            // 尝试创建对象池
            object_pool<test_object> pool(total_allocations);

            // 不应该执行到这里
            std::cout << "FAILED: Constructor should have thrown bad_alloc\n";
            std::abort();
        }
        catch (const std::bad_alloc &)
        {
            std::cout << "PASSED\n";
        }
        allocation_simulator::disable_failure();
    }
}

void test_allocate_allocation_failure()
{
    std::cout << "Running test_allocate_allocation_failure...\n";

    // 测试空池分配失败
    try
    {
        std::cout << "  Testing allocation in empty pool... ";
        allocation_simulator::enable_failure(0); // 第一次分配就失败

        object_pool<test_object> pool(0); // 初始为空
        test_object *obj = pool.allocate();

        std::cout << "FAILED: allocate() should have thrown bad_alloc\n";
        std::abort();
    }
    catch (const std::bad_alloc &)
    {
        std::cout << "PASSED\n";
    }
    allocation_simulator::disable_failure();

    // 测试满池分配失败
    try
    {
        std::cout << "  Testing allocation in full pool... ";
        object_pool<test_object> pool(16); // 初始有一个完整 chunk

        // 填满第一个 chunk
        std::vector<test_object *> objects;
        const size_t block_count = object_pool<test_object>::chunk::block_count;
        for (size_t i = 0; i < block_count; ++i)
        {
            objects.push_back(pool.allocate());
        }

        // 设置分配失败
        allocation_simulator::enable_failure(0);

        // 尝试分配新对象
        test_object *obj = pool.allocate();

        std::cout << "FAILED: allocate() should have thrown bad_alloc\n";
        std::abort();
    }
    catch (const std::bad_alloc &)
    {
        std::cout << "PASSED\n";
    }
    allocation_simulator::disable_failure();
}

void run_exception_safety_tests()
{
    test_constructor_allocation_failure();
    test_allocate_allocation_failure();
}
#endif
// ====================== 异常安全测试结束 ======================

// NOLINTEND

int main()
{
    // 运行正常功能测试
    std::cout << "Running normal functional tests...\n";
    test_object_pool();
    test_destructor_calls();
    test_alignment();
    test_shrink_threshold();
    std::cout << "All normal tests passed!\n";

// 运行异常安全测试（仅在启用时）
#ifdef OBJECT_POOL_EXCEPTION_SAFETY_TESTING
    std::cout << "\nRunning exception safety tests...\n";
    run_exception_safety_tests();
    std::cout << "All exception safety tests passed!\n";
#endif

    std::cout << "\nAll tests completed successfully!\n";
    return 0;
}
// NOLINTEND