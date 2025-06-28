#include <memory_resource>
#include <vector>
#include <list>
#include <chrono>
#include <iostream>
#include <iomanip>

constexpr size_t N = 10'000'000;
constexpr size_t LIST_ITEMS = 100'000;

// 为vector设计的计数分配器
template <typename T>
struct VectorAllocator : std::allocator<T>
{
    size_t *allocation_count;

    VectorAllocator(size_t *count) : allocation_count(count) {}

    template <typename U>
    VectorAllocator(const VectorAllocator<U> &other)
        : allocation_count(other.allocation_count)
    {
    }

    T *allocate(size_t n)
    {
        if (n > this->max_size())
            throw std::bad_alloc();
        if (n > 0)
            ++(*allocation_count);
        return std::allocator<T>::allocate(n);
    }
};

// 测试小对象分配性能（简化版，不再统计链表分配）
void test_small_object_performance()
{
    std::cout << "\n===== 小对象分配性能测试 =====\n";

    // 配置适合小对象的内存池
    std::pmr::pool_options pool_opts;
    pool_opts.max_blocks_per_chunk = 1024;
    pool_opts.largest_required_pool_block = 1024; // 覆盖小对象

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
        std::pmr::unsynchronized_pool_resource pool(pool_opts);
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

    // 配置内存池
    std::pmr::pool_options pool_opts;
    pool_opts.max_blocks_per_chunk = 1024;
    pool_opts.largest_required_pool_block = 1024;

    // 第一阶段：创建并销毁对象
    {
        std::pmr::unsynchronized_pool_resource pool(pool_opts);
        std::pmr::list<int> list(&pool);
        for (size_t i = 0; i < LIST_ITEMS; ++i)
        {
            list.push_back(i);
        }
    }

    // 第二阶段：重用内存
    {
        std::pmr::unsynchronized_pool_resource pool(pool_opts);
        auto start = std::chrono::high_resolution_clock::now();
        std::pmr::list<int> reused_list(&pool);
        for (size_t i = 0; i < LIST_ITEMS; ++i)
        {
            reused_list.push_back(i);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "池分配器重用内存时间: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " ms\n";
    }
}

// 测试vector扩容性能
template <typename Vec>
void benchmark_vector(const char *name)
{
    size_t allocation_count = 0;

    auto start = std::chrono::high_resolution_clock::now();

    if constexpr (std::is_same_v<Vec, std::vector<int>>)
    {
        // 标准vector + 计数分配器
        Vec vec{VectorAllocator<int>{&allocation_count}};
        vec.reserve(N); // 预先分配以避免扩容
        for (size_t i = 0; i < N; ++i)
        {
            vec.push_back(i);
        }
    }
    else
    {
        // pmr::vector + 内存池
        std::pmr::pool_options pool_opts;
        pool_opts.max_blocks_per_chunk = 128;
        pool_opts.largest_required_pool_block = N * sizeof(int); // 覆盖整个vector

        std::pmr::unsynchronized_pool_resource pool(pool_opts);
        Vec vec(&pool);
        vec.reserve(N); // 预先分配以避免扩容
        for (size_t i = 0; i < N; ++i)
        {
            vec.push_back(i);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout
        << std::left << std::setw(35) << name << ": "
        << "时间: " << std::setw(5)
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms, 分配次数: " << allocation_count << "\n";
}

int main()
{
    // 测试小对象性能（简化版）
    test_small_object_performance();

    // 测试内存重用效果
    test_memory_reuse();

    // 测试vector性能
    std::cout << "\n===== Vector 性能测试 =====\n";
    benchmark_vector<std::vector<int>>("std::vector (default allocator)");
    benchmark_vector<std::pmr::vector<int>>("std::pmr::vector (pool allocator)");

    // NOTE: std::pmr::vector 还是 比不了一点点 std::vector
    return 0;
}