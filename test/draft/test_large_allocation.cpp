#include <memory_resource>
#include <vector>
#include <thread>
#include <atomic>
#include <iostream>
#include <chrono>

constexpr int N = 100'000'000;

struct BadCounter
{
    int a;
    int b;
}; // a和b可能在同一缓存行
struct GoodCounter
{
    alignas(64) int a;
    alignas(64) int b;
}; // 强制隔离

void test_counter(auto &counter)
{
    auto start = std::chrono::high_resolution_clock::now();
    std::thread t1([&] {
        for (int i = 0; i < N; ++i)
            counter.a++;
    });
    std::thread t2([&] {
        for (int i = 0; i < N; ++i)
            counter.b++;
    });
    t1.join();
    t2.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout
        << "耗时: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
        << "ms\n";
}

int main()
{
    // 使用pmr确保内存对齐
    std::pmr::unsynchronized_pool_resource pool;
    auto *bad = (BadCounter *)pool.allocate(sizeof(BadCounter));
    auto *good = (GoodCounter *)pool.allocate(sizeof(GoodCounter));

    std::cout << "错误共享测试（未隔离）: ";
    test_counter(*bad); // 预期较慢

    std::cout << "缓存优化测试（隔离）: ";
    test_counter(*good); // 预期较快
}