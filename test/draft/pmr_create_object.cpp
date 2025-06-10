#include <cassert>
#include <cstddef>
#include <memory>
#include <memory_resource>
#include <iostream>
#include <utility>

// NOLINTBEGIN

// 创建对象：分配内存 + 构造
template <typename T, typename... Args>
T *create_object(std::pmr::memory_resource &pool, Args &&...args)
{
    void *mem = pool.allocate(sizeof(T), alignof(T));
    try
    {
        std::cout << "<<< placement new" << "\n";
        return new (mem) T(std::forward<Args>(args)...); // placement new
    }
    catch (...)
    {
        pool.deallocate(mem, sizeof(T), alignof(T));
        throw;
    }
}

// 销毁对象：析构 + 释放内存
template <typename T>
void destroy_object(std::pmr::memory_resource &pool, T *obj) noexcept
{
    if (obj)
    {
        std::cout << "<<< obj->~T() start" << "\n";
        obj->~T(); // 显式析构
        std::cout << "<<< obj->~T() end" << "\n";
        pool.deallocate(obj, sizeof(T), alignof(T));
    }
}

// 测试类
struct Widget
{
    int id;
    std::string name;

    Widget(int id, std::string name) : id(id), name(std::move(name))
    {
        std::cout << "Constructed Widget #" << id << "\n";
    }

    ~Widget()
    {
        std::cout << "Destroyed Widget #" << id << "\n";
    }
};

template <typename T, typename... Args>
class PoolPtr
{
  public:
    PoolPtr(std::pmr::memory_resource &pool, Args &&...args)
        : pool_(&pool), ptr_(create_object<T>(pool, std::forward<Args>(args)...))
    {
    }

    ~PoolPtr()
    {
        if (ptr_)
            destroy_object(*pool_, ptr_);
    }

    // 禁止复制（示例简化版）
    T *operator->() const
    {
        return ptr_;
    }
    T &operator*() const
    {
        return *ptr_;
    }

  private:
    std::pmr::memory_resource *pool_;
    T *ptr_;
};
// template <typename T, typename... Args>
// PoolPtr(std::pmr::memory_resource &, Args &&...) -> PoolPtr<T, Args...>;

template <typename T, typename... Args>
PoolPtr<T, Args...> make_pool_ptr(std::pmr::memory_resource &pool, Args &&...args)
{
    return PoolPtr<T, Args...>(pool, std::forward<Args>(args)...);
}

void test_ptr() noexcept
{
    {
        int *p1 = new int(42);
        int *p2 = std::move(p1); // p2 现在指向 42，p1 仍然指向 42（未置空）

        assert(p1 == p2);
        assert(*p1 == *p2); // NOTE: 基本类型，都是值语义。明确语义请智能指针

        struct A
        {
            int a{0};
            int *ptr{nullptr};
        };
        // NOTE: c++ 默认不会帮你做 多余的事情
        A obj{*p2, p2};
        assert((ptrdiff_t)(obj.ptr) == (ptrdiff_t)p2);
        A o = std::move(obj);
        assert((ptrdiff_t)(obj.ptr) == (ptrdiff_t)p2);
        assert((ptrdiff_t)(o.ptr) == (ptrdiff_t)p2);

        // NOTE: 默认移动 不会帮你的成员指针 置为 nullptr
        assert(obj.ptr != nullptr);
    }
    {
        std::unique_ptr<int> p1 = std::make_unique<int>(42);
        std::unique_ptr<int> p2 = std::move(p1); // p1 现在为 nullptr
        assert(p1 == nullptr);                   // NOTE: 语义工程化 请智能指针

        struct Widget
        {
            int *raw_ptr;
            std::unique_ptr<int> smart_ptr;
        };

        Widget w1;
        w1.raw_ptr = new int(42);
        w1.smart_ptr = std::make_unique<int>(100);

        Widget w2 = std::move(w1);
        // w1.raw_ptr 仍然指向 42（未置空）
        assert(*(w1.raw_ptr) == 42); // NOTE: 基本类型 默认值语义
        // w1.smart_ptr 现在是 nullptr（被置空）
        assert(w1.smart_ptr == nullptr); // NOTE: 库 拥有工程化实践

        class MyPtr
        {
            int *ptr;

          public:
            MyPtr(int *p) noexcept : ptr(p) {}
            MyPtr(MyPtr &&other) noexcept : ptr(other.ptr)
            {
                other.ptr = nullptr; // 手动置空
            }
            ~MyPtr() noexcept
            {
                delete ptr;
            }
            auto *get_ptr() noexcept
            {
                return ptr;
            }
        };
        MyPtr my_ptr{new int{2}};
        auto ptr = std::move(my_ptr);
        assert(my_ptr.get_ptr() == nullptr);
    }

    // NOTE: DELETE nullptr 没作用
    int *ptr = nullptr;
    delete ptr;
    delete[] ptr;
    static auto call0 = false;
    static auto call1 = false;
    struct object
    {
        object() noexcept
        {
            std::cout << " object() " << "\n";
            call0 = true;
        }
        ~object() noexcept
        {
            std::cout << " ~object() " << "\n";
            call1 = true;
        }
    };
    // NOTE: 仅仅申请和释放
    void *memory = ::operator new(sizeof(object)); // 仅分配内存，不构造对象
    ::operator delete(memory);
    assert(not call0);
    assert(not call1);

    {
        using T = object;
        std::allocator<T> alloc;
        T *memory = alloc.allocate(1); // 分配 1 个 T 的内存，不构造对象
                                       // 手动构造对象（可选）
        std::cout << " construct_at() " << (ptrdiff_t)memory << "\n";
        std::construct_at(memory); // C++20 起
                                   // 手动析构对象（可选）
        std::cout << " destroy_at() " << (ptrdiff_t)memory << "\n";
        std::destroy_at(memory); // C++17 起
        // 释放内存
        alloc.deallocate(memory, 1);
    }

    std::cout << "\n";
}

int main()
{
    test_ptr();
    // 创建内存池（使用默认上游资源）
    std::pmr::unsynchronized_pool_resource pool;

    // 创建对象
    Widget *w1 = create_object<Widget>(pool, 1, "Alice");
    Widget *w2 = create_object<Widget>(pool, 2, "Bob");

    // 使用对象
    std::cout << "Using widget: " << w1->name << "\n";
    std::cout << "Using widget: " << w2->name << "\n";

    // 销毁对象
    destroy_object(pool, w1);
    destroy_object(pool, w2);

    {
        Widget *w3 = create_object<Widget>(pool, 1, "Alice");
        Widget *w4 = create_object<Widget>(pool, 2, "Bob");
        destroy_object(pool, w3);
        destroy_object(pool, w4);

        // NOTE: 重用的证据
        assert(ptrdiff_t(w1) == ptrdiff_t(w3));
        assert(ptrdiff_t(w2) == ptrdiff_t(w4));
    }

    // NOTE: 可以RAII 设置
    auto ptr = make_pool_ptr<Widget>(pool, 3, "make_pool_ptr");

    // 内存池会在析构时释放所有内存
    std::cout << "\n========main end=======\n";
    return 0;
}
// NOLINTEND