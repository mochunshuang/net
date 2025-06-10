#include <memory_resource>
#include <vector>
#include <chrono>
#include <iostream>

// NOLINTBEGIN

constexpr size_t N = 1'000'000;

// 测试函数模板
template <typename Alloc>
void test_allocator(const char *name)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<int, Alloc> vec;
    for (size_t i = 0; i < N; ++i)
    {
        vec.push_back(i);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << name << ": "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n";
}

template <typename Vec>
void benchmark(const char *name, Vec &vec)
{
    constexpr size_t N = 10'000'000; // 测试数据量

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i)
    {
        vec.push_back(i); // 模拟实际使用场景
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout
        << name << " time: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << " ms\n";
}

int main()
{
    // 1. 标准分配器
    test_allocator<std::allocator<int>>("std::allocator");

    // 2. Boost 池分配器
    // test_allocator<boost::pool_allocator<int>>("boost::pool_allocator");

    // 3. PMR + unsynchronized_pool_resource
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::polymorphic_allocator<int> pool_alloc(&pool);
    test_allocator<decltype(pool_alloc)>("pmr::polymorphic_allocator + pool");

    // 4. PMR + monotonic_buffer_resource（预分配 1MB）
    char buffer[1000];
    std::pmr::monotonic_buffer_resource mono_buf(buffer, sizeof(buffer));
    std::pmr::polymorphic_allocator<int> mono_alloc(&mono_buf);
    test_allocator<decltype(mono_alloc)>("pmr::polymorphic_allocator + monotonic");

    std::cout << "\n ===== vector + allocator\n";
    {
        // 1. 准备 PMR 内存资源（选择一种测试）
        // std::pmr::monotonic_buffer_resource pool; // 单调缓冲区（分配极快，但不能释放）
        std::pmr::unsynchronized_pool_resource pool; // 内存池（适合小对象频繁分配）

        // 2. 测试 std::vector + 默认分配器（基线）
        std::vector<int> std_vec;
        benchmark("std::vector (default allocator)", std_vec);

        // NOTE: pmr::vector 的内存布局 + unsynchronized_pool_resource 性能极速下载
        //  3. 测试 std::pmr::vector + PMR 分配器
        {
            std::pmr::vector<int> pmr_vec(&pool);
            benchmark("times0: std::pmr::vector (PMR allocator)", pmr_vec);

            std::pmr::vector<int> pmr_vec2(&pool);
            benchmark("times0-0: std::pmr::vector (PMR allocator)", pmr_vec2);
        }
        {
            std::pmr::vector<int> pmr_vec(&pool);
            benchmark("times1: std::pmr::vector (PMR allocator)", pmr_vec);
        }
        {
            pool.release();
            std::pmr::vector<int> pmr_vec(&pool);
            benchmark("times2: std::pmr::vector (PMR allocator)", pmr_vec);
        }
        // NOTE: 结论是 unsynchronized_pool_resource + pmr::vector 没有重复使用的可能

        // 4. 修正：正确初始化 std::vector + PMR 分配器
        std::pmr::polymorphic_allocator<int> pmr_alloc(&pool);
        std::vector<int, std::pmr::polymorphic_allocator<int>> std_vec_pmr(pmr_alloc);
        benchmark("std::vector + PMR allocator (对比用)", std_vec_pmr);
    }

    // NOTE: 测试结果证明了，std::pmr集合是性能低下的，无法优化。不要使用

    return 0;
}
// NOLINTEND