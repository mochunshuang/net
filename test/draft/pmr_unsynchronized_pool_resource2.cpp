#include <iostream>
#include <memory_resource>
#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <random>

// NOLINTBEGIN

// 计时器类
class Timer
{
  public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}

    double elapsed() const
    {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

// 测试不同大小的对象
template <size_t ObjectSize>
struct TestObject
{
    alignas(16) char data[ObjectSize];
};

// 直接使用new/delete的测试函数 - 批量模式
template <typename T>
void test_new_delete_batch(size_t iterations, size_t batch_size,
                           std::atomic<double> &total_time)
{
    Timer timer;
    for (size_t i = 0; i < iterations; ++i)
    {
        std::vector<T *> batch;
        batch.reserve(batch_size);

        // 分配一批对象
        for (size_t j = 0; j < batch_size; ++j)
        {
            batch.push_back(new T);
        }

        // 使用对象 (模拟)
        for (auto obj : batch)
        {
            // 简单使用对象，避免优化掉循环
            obj->data[0] = static_cast<char>(i % 256);
        }

        // 释放这批对象
        for (auto obj : batch)
        {
            delete obj;
        }
    }
    total_time = timer.elapsed();
}

// 使用unsynchronized_pool_resource的测试函数 - 批量模式
template <typename T>
void test_pool_resource_batch(size_t iterations, size_t batch_size,
                              std::atomic<double> &total_time)
{
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::polymorphic_allocator<T> alloc(&pool);

    Timer timer;
    for (size_t i = 0; i < iterations; ++i)
    {
        std::vector<T *, std::pmr::polymorphic_allocator<T *>> batch(alloc);
        batch.reserve(batch_size);

        // 分配一批对象
        for (size_t j = 0; j < batch_size; ++j)
        {
            batch.push_back(alloc.template new_object<T>());
        }

        // 使用对象 (模拟)
        for (auto obj : batch)
        {
            obj->data[0] = static_cast<char>(i % 256);
        }

        // 释放这批对象
        for (auto obj : batch)
        {
            alloc.delete_object(obj);
        }
    }
    total_time = timer.elapsed();
}

// 使用unsynchronized_pool_resource的测试函数 - 复用模式
template <typename T>
void test_pool_resource_reuse(size_t iterations, size_t batch_size,
                              std::atomic<double> &total_time)
{
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::polymorphic_allocator<T> alloc(&pool);

    // 预先分配一批对象
    std::vector<T *, std::pmr::polymorphic_allocator<T *>> batch(alloc);
    batch.reserve(batch_size);

    for (size_t j = 0; j < batch_size; ++j)
    {
        batch.push_back(alloc.template new_object<T>());
    }

    Timer timer;
    for (size_t i = 0; i < iterations; ++i)
    {
        // 复用对象
        for (auto obj : batch)
        {
            obj->data[0] = static_cast<char>(i % 256);
        }
    }

    // 释放对象
    for (auto obj : batch)
    {
        alloc.delete_object(obj);
    }

    total_time = timer.elapsed();
}

int main()
{
    // 测试参数：每轮测试执行10万次操作
    const size_t iterations = 100000;
    // 每批分配100个对象
    const size_t batch_size = 100;

    std::cout << "测试开始：每种测试将执行 " << iterations << " 次操作，每批 "
              << batch_size << " 个对象" << std::endl;
    std::cout << "=========================================" << std::endl;

    // 测试小型对象 (32字节)
    std::cout << "测试小型对象 (32字节):" << std::endl;

    std::atomic<double> new_delete_small_time;
    test_new_delete_batch<TestObject<32>>(iterations, batch_size, new_delete_small_time);
    std::cout << "new/delete (批量模式) 耗时: " << new_delete_small_time << " ms"
              << std::endl;

    std::atomic<double> pool_small_time;
    test_pool_resource_batch<TestObject<32>>(iterations, batch_size, pool_small_time);
    std::cout << "对象池 (批量模式) 耗时: " << pool_small_time << " ms" << std::endl;
    std::cout << "性能提升: " << (1 - pool_small_time / new_delete_small_time) * 100
              << "%" << std::endl;

    std::atomic<double> pool_reuse_small_time;
    test_pool_resource_reuse<TestObject<32>>(iterations, batch_size,
                                             pool_reuse_small_time);
    std::cout << "对象池 (复用模式) 耗时: " << pool_reuse_small_time << " ms"
              << std::endl;
    std::cout << "相对于new/delete的性能提升: "
              << (1 - pool_reuse_small_time / new_delete_small_time) * 100 << "%"
              << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    // 测试中型对象 (256字节)
    std::cout << "测试中型对象 (256字节):" << std::endl;

    std::atomic<double> new_delete_medium_time;
    test_new_delete_batch<TestObject<256>>(iterations, batch_size,
                                           new_delete_medium_time);
    std::cout << "new/delete (批量模式) 耗时: " << new_delete_medium_time << " ms"
              << std::endl;

    std::atomic<double> pool_medium_time;
    test_pool_resource_batch<TestObject<256>>(iterations, batch_size, pool_medium_time);
    std::cout << "对象池 (批量模式) 耗时: " << pool_medium_time << " ms" << std::endl;
    std::cout << "性能提升: " << (1 - pool_medium_time / new_delete_medium_time) * 100
              << "%" << std::endl;

    std::atomic<double> pool_reuse_medium_time;
    test_pool_resource_reuse<TestObject<256>>(iterations, batch_size,
                                              pool_reuse_medium_time);
    std::cout << "对象池 (复用模式) 耗时: " << pool_reuse_medium_time << " ms"
              << std::endl;
    std::cout << "相对于new/delete的性能提升: "
              << (1 - pool_reuse_medium_time / new_delete_medium_time) * 100 << "%"
              << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    // 测试大型对象 (4096字节)
    std::cout << "测试大型对象 (4096字节):" << std::endl;

    std::atomic<double> new_delete_large_time;
    test_new_delete_batch<TestObject<4096>>(iterations, batch_size,
                                            new_delete_large_time);
    std::cout << "new/delete (批量模式) 耗时: " << new_delete_large_time << " ms"
              << std::endl;

    std::atomic<double> pool_large_time;
    test_pool_resource_batch<TestObject<4096>>(iterations, batch_size, pool_large_time);
    std::cout << "对象池 (批量模式) 耗时: " << pool_large_time << " ms" << std::endl;
    std::cout << "性能提升: " << (1 - pool_large_time / new_delete_large_time) * 100
              << "%" << std::endl;

    std::atomic<double> pool_reuse_large_time;
    test_pool_resource_reuse<TestObject<4096>>(iterations, batch_size,
                                               pool_reuse_large_time);
    std::cout << "对象池 (复用模式) 耗时: " << pool_reuse_large_time << " ms"
              << std::endl;
    std::cout << "相对于new/delete的性能提升: "
              << (1 - pool_reuse_large_time / new_delete_large_time) * 100 << "%"
              << std::endl;

    // NOTE: 明确知道复用的时候，请使用对象池。
    return 0;
}
// NOLINTEND