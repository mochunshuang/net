#include <memory_resource>
#include <vector>
#include <list>
#include <chrono>
#include <iostream>

// NOLINTBEGIN

constexpr size_t N = 1'000'000;
constexpr size_t LIST_ITEMS = 100'000;

// 测试小对象分配性能
void test_small_object_performance()
{
    std::cout << "\n===== 小对象分配性能测试 =====\n";

    // 1. 标准分配器：链表
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::list<int> list;
        for (size_t i = 0; i < LIST_ITEMS; ++i)
        {
            list.push_back(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "std::list (default allocator): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
    }

    // 2. 池分配器：链表
    {
        std::pmr::unsynchronized_pool_resource pool;
        auto start = std::chrono::high_resolution_clock::now();
        std::pmr::list<int> list(&pool);
        for (size_t i = 0; i < LIST_ITEMS; ++i)
        {
            list.push_back(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "pmr::list (pool allocator): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
    }
}

// 测试重用效果
void test_memory_reuse()
{
    std::cout << "\n===== 内存重用效果测试 =====\n";

    std::pmr::unsynchronized_pool_resource pool;

    // 第一阶段：创建并销毁对象
    {
        std::pmr::list<int> list(&pool);
        for (size_t i = 0; i < LIST_ITEMS; ++i)
        {
            list.push_back(i);
        }
        // list 析构时内存返回池中
    }

    // 第二阶段：重用内存
    auto start = std::chrono::high_resolution_clock::now();
    {
        std::pmr::list<int> reused_list(&pool);
        for (size_t i = 0; i < LIST_ITEMS; ++i)
        {
            reused_list.push_back(i);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout
        << "池分配器重用内存时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n";
}

// 改进的 vector 测试：显示扩容次数
template <typename Vec>
void benchmark_with_reserve(const char *name)
{
    constexpr size_t N = 10'000'000;
    size_t allocations = 0;

    // 拦截分配操作
    struct CountingResource : std::pmr::memory_resource
    {
        std::pmr::memory_resource *upstream;
        size_t *counter;

        CountingResource(std::pmr::memory_resource *u, size_t *c)
            : upstream(u), counter(c)
        {
        }

        void *do_allocate(size_t bytes, size_t align) override
        {
            ++(*counter);
            return upstream->allocate(bytes, align);
        }

        void do_deallocate(void *p, size_t bytes, size_t align) override
        {
            upstream->deallocate(p, bytes, align);
        }

        bool do_is_equal(const memory_resource &other) const noexcept override
        {
            return this == &other;
        }
    };

    size_t counter = 0;
    CountingResource counting_res(std::pmr::new_delete_resource(), &counter);

    Vec vec;
    if constexpr (std::is_same_v<Vec, std::pmr::vector<int>>)
    {
        // 使用计数资源
        std::pmr::unsynchronized_pool_resource pool(&counting_res);
        vec = Vec(&pool);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i)
    {
        vec.push_back(i);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout
        << name << ":\n"
        << "  时间: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n"
        << "  扩容次数: " << counter << "\n\n";
}

int main()
{

    //  测试小对象性能
    test_small_object_performance();

    // 测试内存重用效果
    test_memory_reuse();

    // 显示扩容次数的 vector 测试
    std::cout << "\n===== Vector 扩容分析 =====\n";
    benchmark_with_reserve<std::vector<int>>("std::vector (default allocator)");

    std::pmr::unsynchronized_pool_resource pool;
    benchmark_with_reserve<std::pmr::vector<int>>("std::pmr::vector (pool allocator)");

    // NOTE: std::pmr的集合 性能低下，这就是结论

    return 0;
}
/*

===== 小对象分配性能测试 =====
std::list (default allocator): 11 ms
pmr::list (pool allocator): 19 ms

===== 内存重用效果测试 =====
池分配器重用内存时间: 25 ms

===== Vector 扩容分析 =====
std::vector (default allocator):
  时间: 391 ms
  扩容次数: 0

std::pmr::vector (pool allocator):
  时间: 2982 ms
  扩容次数: 1
*/
// NOLINTEND