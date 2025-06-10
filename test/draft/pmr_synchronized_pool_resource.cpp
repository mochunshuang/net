#include <memory_resource>
#include <vector>
#include <iostream>

// NOLINTBEGIN

struct Person
{
    std::string name;
    int age;
    // Person(std::string n, int a) : name(std::move(n)), age(a)
    // {
    //     std::cout << "Constructing " << name << "\n";
    // }
    // ~Person()
    // {
    //     std::cout << "Destructing " << name << "\n";
    // }
};

class MemoryPool
{
    std::pmr::synchronized_pool_resource pool;

  public:
    // 分配未初始化内存
    void *allocate(size_t size)
    {
        return pool.allocate(size);
    }

    // 释放内存
    void deallocate(void *p, size_t size)
    {
        pool.deallocate(p, size);
    }

    // 在未初始化内存上构造对象
    template <typename T, typename... Args>
    T *construct(Args &&...args)
    {
        void *mem = allocate(sizeof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }

    // 销毁对象并释放内存
    template <typename T>
    void destroy(T *p)
    {
        p->~T();
        deallocate(p, sizeof(T));
    }
};

class MemoryPool2
{
    std::pmr::unsynchronized_pool_resource pool;

  public:
    // 分配未初始化内存
    void *allocate(size_t size)
    {
        return pool.allocate(size);
    }

    // 释放内存
    void deallocate(void *p, size_t size)
    {
        pool.deallocate(p, size);
    }

    // 在未初始化内存上构造对象
    template <typename T, typename... Args>
    T *construct(Args &&...args)
    {
        void *mem = allocate(sizeof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }

    // 销毁对象并释放内存
    template <typename T>
    void destroy(T *p)
    {
        p->~T();
        deallocate(p, sizeof(T));
    }
};

void test_new_delete(size_t count)
{
    std::vector<Person *> ptrs;
    ptrs.reserve(count);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        ptrs.push_back(new Person("Test", i));
    }
    for (auto p : ptrs)
    {
        delete p;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "new/delete: " << (end - start).count() << " ns\n";
}

void test_memory_pool(size_t count)
{
    MemoryPool pool;
    std::vector<Person *> ptrs;
    ptrs.reserve(count);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        ptrs.push_back(pool.construct<Person>("Test", i));
    }
    for (auto p : ptrs)
    {
        pool.destroy(p);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "MemoryPool: " << (end - start).count() << " ns\n";
}
void test_memory_pool2(size_t count)
{
    MemoryPool2 pool;

    std::vector<Person *> ptrs;
    ptrs.reserve(count);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        ptrs.push_back(pool.construct<Person>("Test", i));
    }
    for (auto p : ptrs)
    {
        pool.destroy(p);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "MemoryPool2: " << (end - start).count() << " ns\n";
}

int main()
{
    {
        struct Person
        {
            std::string name;
            int age;
            Person(std::string n, int a) : name(std::move(n)), age(a)
            {
                std::cout << "Constructing " << name << "\n";
            }
            ~Person()
            {
                std::cout << "Destructing " << name << "\n";
            }
        };
        MemoryPool pool;
        // 分配并构造对象
        Person *p1 = pool.construct<Person>("Alice", 30);
        Person *p2 = pool.construct<Person>("Bob", 25);
        // 使用对象
        std::cout << p1->name << " " << p1->age << "\n";
        // 销毁对象
        pool.destroy(p1);
        pool.destroy(p2);
    }

    constexpr size_t TEST_COUNT = 100000;
    test_new_delete(TEST_COUNT);
    test_memory_pool(TEST_COUNT);
    test_memory_pool2(TEST_COUNT);

    /*
//NOTE: 封装 unsynchronized_pool_resource  就能建立成 内存池。 比 new / delete 优先
//NOTE: unsynchronized_pool_resource 只会 挑【一个内存块】来响应 内存分配请求
    new/delete: 13722800 ns
    MemoryPool: 19193900 ns
    MemoryPool2: 13423400 ns
    */
    return 0;
}
// NOLINTEND