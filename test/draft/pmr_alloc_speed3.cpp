#include <memory_resource>
#include <vector>
#include <chrono>
#include <iostream>
#include <memory>
#include <cstdlib>

// NOLINTBEGIN

constexpr size_t ALLOC_COUNT = 1000;
constexpr size_t BLOCK_SIZE = 64; // 典型小对象大小

// 直接测试分配器性能
void test_allocator_performance()
{
    std::cout << "\n===== 分配器核心性能测试 =====\n";

    // 1. 系统默认分配器 (malloc/free)
    {
        auto start = std::chrono::high_resolution_clock::now();
        void *blocks[ALLOC_COUNT];

        for (size_t i = 0; i < ALLOC_COUNT; ++i)
        {
            blocks[i] = std::malloc(BLOCK_SIZE);
        }

        for (size_t i = 0; i < ALLOC_COUNT; ++i)
        {
            std::free(blocks[i]);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "系统默认分配器 (malloc/free): "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }

    // 2. 标准库分配器
    {
        std::allocator<std::byte> alloc;
        auto start = std::chrono::high_resolution_clock::now();
        std::byte *blocks[ALLOC_COUNT];

        for (size_t i = 0; i < ALLOC_COUNT; ++i)
        {
            blocks[i] = alloc.allocate(BLOCK_SIZE);
        }

        for (size_t i = 0; i < ALLOC_COUNT; ++i)
        {
            alloc.deallocate(blocks[i], BLOCK_SIZE);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "std::allocator: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }

    // 3. PMR 池分配器
    {
        std::pmr::unsynchronized_pool_resource pool;
        auto start = std::chrono::high_resolution_clock::now();
        void *blocks[ALLOC_COUNT];

        for (size_t i = 0; i < ALLOC_COUNT; ++i)
        {
            blocks[i] = pool.allocate(BLOCK_SIZE);
        }

        for (size_t i = 0; i < ALLOC_COUNT; ++i)
        {
            pool.deallocate(blocks[i], BLOCK_SIZE);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "pmr::unsynchronized_pool_resource: "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }
    // NOTE: unsynchronized_pool_resource 最快
}

// 优化后的 vector 测试
void test_vector_performance()
{
    std::cout << "\n===== Vector 性能对比 =====\n";

    constexpr size_t N = 10'000'000;

    // 1. 标准 vector
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<int> vec;
        vec.reserve(N); // 预分配避免扩容

        for (size_t i = 0; i < N; ++i)
        {
            vec.push_back(static_cast<int>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "std::vector (预分配): "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }

    // 2. PMR vector 使用池分配器
    {
        std::pmr::unsynchronized_pool_resource pool;
        auto start = std::chrono::high_resolution_clock::now();

        std::pmr::vector<int> vec(&pool);
        vec.reserve(N); // 同样预分配

        for (size_t i = 0; i < N; ++i)
        {
            vec.push_back(static_cast<int>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "pmr::vector + 池分配器 (预分配): "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }

    // PMR vector 使用单调缓冲区 +  static
    {
        // 1 048 576
        // NOTE: 负优化。没想到吧。但比pmr::vector +池分配器，强一点点，比直接new差
        constexpr size_t BUFFER_SIZE = 1024 * 1024; // NOTE: 有点优化 但是不多
        char buffer[BUFFER_SIZE];

        std::pmr::monotonic_buffer_resource mono_buf{buffer, BUFFER_SIZE,
                                                     std::pmr::new_delete_resource()};

        auto start = std::chrono::high_resolution_clock::now();

        std::pmr::vector<int> vec(&mono_buf);
        vec.reserve(N); // 预分配

        for (size_t i = 0; i < N; ++i)
        {
            vec.push_back(static_cast<int>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "pmr::vector + 一点 static 单调缓冲区 (预分配): "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";

        mono_buf.release();
    }
    {
        // 创建足够大的缓冲区（避免溢出到上游）
        constexpr size_t BUFFER_SIZE = N * sizeof(int) * 1.5; // 额外50%空间
        std::unique_ptr<char[]> buffer(new char[BUFFER_SIZE]);

        std::pmr::monotonic_buffer_resource mono_buf{buffer.get(), BUFFER_SIZE,
                                                     std::pmr::new_delete_resource()};

        auto start = std::chrono::high_resolution_clock::now();

        std::pmr::vector<int> vec(&mono_buf);
        vec.reserve(N); // 预分配

        for (size_t i = 0; i < N; ++i)
        {
            vec.push_back(static_cast<int>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout
            << "pmr::vector + new 单调缓冲区 (预分配): "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }

    // 4. 避免 PMR 开销的直接访问
    {
        std::pmr::unsynchronized_pool_resource pool;
        auto start = std::chrono::high_resolution_clock::now();

        // 直接操作内存，避免 vector 开销
        int *data = static_cast<int *>(pool.allocate(N * sizeof(int)));

        for (size_t i = 0; i < N; ++i)
        {
            data[i] = static_cast<int>(i);
        }

        pool.deallocate(data, N * sizeof(int));
        auto end = std::chrono::high_resolution_clock::now();

        std::cout
            << "直接使用池分配器 (数组): "
            << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()
            << " ns\n";
    }
    // NOTE: std::vector 优于 std::pmr::vector 。提前预分配也不过
    // NOTE: 使用 std::pmr::集合 不如自己 new /delete。不过如果这样不如直接 std::vector
}

int main()
{
    test_allocator_performance();
    test_vector_performance();
    return 0;
}
// NOLINTEND