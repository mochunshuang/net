#include <memory_resource>
#include <vector>
#include <chrono>
#include <iostream>

// NOLINTBEGIN
auto test_new_delete(size_t count, size_t size) noexcept
{
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        void *p = new char[size];
        delete[] static_cast<char *>(p);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto times = (end - start).count();
    std::cout << "new/delete: " << times << " ns\n";
    return times;
}

auto test_monotonic_buffer(size_t count, size_t size) noexcept
{
    char buffer[1024 * 1024]; // 1MB 缓冲区
    std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)};
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        void *p = pool.allocate(size);
    }
    pool.release(); // 整体释放
    auto end = std::chrono::high_resolution_clock::now();
    auto times = (end - start).count();
    std::cout << "monotonic_buffer: " << times << " ns\n";
    return times;
}

auto test_unsynchronized_pool(size_t count, size_t size) noexcept
{
    char buffer[1024 * 1024]; // 1MB 缓冲区
    std::pmr::unsynchronized_pool_resource pool;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i)
    {
        void *p = pool.allocate(size);
    }
    pool.release(); // 整体释放
    auto end = std::chrono::high_resolution_clock::now();
    auto times = (end - start).count();
    std::cout << "unsynchronized_pool: " << times << " ns\n";
    return times;
}

int main()
{
    constexpr size_t COUNT = 100000;
    constexpr size_t SMALL_SIZE = 16;   // 小对象
    constexpr size_t LARGE_SIZE = 1024; // 大对象

    // NOTE: 差个 7 八倍。 这就是 堆内存 和 栈内存的区别吗？

    std::cout << "=== Small Objects (16B) ===\n";
    auto t0 = test_new_delete(COUNT, SMALL_SIZE);
    auto t1 = test_monotonic_buffer(COUNT, SMALL_SIZE);
    auto t2 = test_unsynchronized_pool(COUNT, SMALL_SIZE);
    std::cout << "=== Small Objects (16B): t0/t1: " << (1.0 * t0) / t1 << "\n";
    std::cout << "=== Small Objects (16B): t0/t2: " << (1.0 * t0) / t2 << "\n";
    std::cout << "=== Small Objects (16B): t1/t2: " << (1.0 * t1) / t2 << "\n";

    std::cout << "\n=== Large Objects (1KB) ===\n";
    t0 = test_new_delete(COUNT, LARGE_SIZE);
    t1 = test_monotonic_buffer(COUNT, LARGE_SIZE);
    t2 = test_unsynchronized_pool(COUNT, LARGE_SIZE);
    std::cout << "=== Large Objects (1KB): t0/t1: " << (1.0 * t0) / t1 << "\n";
    std::cout << "=== Large Objects (1KB): t0/t2: " << (1.0 * t0) / t2 << "\n";
    std::cout << "=== Large Objects (1KB): t1/t2: " << (1.0 * t1) / t2 << "\n";

    // NOTE: 比值越大 性能提示越高。 比值 > 1。分母的 性能高
    // NOTE: monotonic_buffer_resource > unsynchronized_pool_resource > new/delete

    return 0;
}
/*
//NOTE: new / delete 16B 的  耗时 比  1KB 多
=== Small Objects (16B) ===
new/delete: 6204400 ns
monotonic_buffer: 730400 ns
=== Small Objects (16B): t0/t1: 8

=== Large Objects (1KB) ===
new/delete: 5761400 ns
monotonic_buffer: 880500 ns
=== Large Objects (1KB): t0/t1: 6

*/
// NOLINTEND