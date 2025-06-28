#include <iostream>
#include <vector>
#include <chrono>
#include <new>
#include <thread>
#include <atomic>
#include <latch>
#include <iomanip>

// NOLINTBEGIN

constexpr size_t OBJECT_SIZE = 1024;
constexpr size_t OBJECT_COUNT = 1000;
constexpr size_t ITERATIONS = 100;
constexpr size_t THREAD_COUNT = 4;

// 基准对象
struct BaseObject
{
    char data[OBJECT_SIZE];
};

// 分配策略枚举
enum class AlignmentStrategy
{
    None,       // 无特殊对齐
    ObjectSize, // 对象大小对齐
    CacheLine   // 缓存行对齐
};

// 通用分配函数
void *allocate_object(AlignmentStrategy strategy, size_t size = sizeof(BaseObject))
{
    switch (strategy)
    {
    case AlignmentStrategy::ObjectSize:
        return ::operator new(size, static_cast<std::align_val_t>(OBJECT_SIZE));

    case AlignmentStrategy::CacheLine:
        return ::operator new(size, static_cast<std::align_val_t>(64));

    case AlignmentStrategy::None:
    default:
        return new char[size];
    }
}

// 通用释放函数
void deallocate_object(void *ptr, AlignmentStrategy strategy)
{
    switch (strategy)
    {
    case AlignmentStrategy::ObjectSize:
        ::operator delete(ptr, static_cast<std::align_val_t>(OBJECT_SIZE));
        break;

    case AlignmentStrategy::CacheLine:
        ::operator delete(ptr, static_cast<std::align_val_t>(64));
        break;

    case AlignmentStrategy::None:
    default:
        delete[] static_cast<char *>(ptr);
    }
}

// 性能测试函数
void test_performance(AlignmentStrategy strategy, const std::string &strategy_name)
{
    std::vector<BaseObject *> objects;
    objects.reserve(OBJECT_COUNT);

    // 分配对象
    for (size_t i = 0; i < OBJECT_COUNT; ++i)
    {
        objects.push_back(static_cast<BaseObject *>(allocate_object(strategy)));
    }

    // 验证对齐
    size_t aligned_count = 0;
    size_t alignment = 0;

    switch (strategy)
    {
    case AlignmentStrategy::ObjectSize:
        alignment = OBJECT_SIZE;
        break;
    case AlignmentStrategy::CacheLine:
        alignment = 64;
        break;
    default:
        alignment = alignof(void *); // 基础对齐
    }

    for (BaseObject *obj : objects)
    {
        uintptr_t addr = reinterpret_cast<uintptr_t>(obj);
        if (addr % alignment == 0)
            aligned_count++;
    }

    std::cout << "\n[" << strategy_name << "策略]\n";
    std::cout << "  对齐要求: " << alignment << " 字节\n";
    std::cout << "  对齐验证: " << aligned_count << "/" << OBJECT_COUNT << " ("
              << std::fixed << std::setprecision(1)
              << (aligned_count * 100.0 / OBJECT_COUNT) << "%)\n";

    // 内存访问测试
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t iter = 0; iter < ITERATIONS; ++iter)
    {
        for (BaseObject *obj : objects)
        {
            for (size_t j = 0; j < OBJECT_SIZE; ++j)
            {
                obj->data[j] = static_cast<char>(j + iter);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "  访问耗时: " << duration.count() << " μs\n";

    // 释放对象
    for (BaseObject *obj : objects)
    {
        deallocate_object(obj, strategy);
    }
}

// 多线程伪共享测试函数
void test_false_sharing(AlignmentStrategy strategy, const std::string &strategy_name)
{
    constexpr size_t ITERS = 10'000'000;

    // 定义基于策略的对齐结构
    struct Counter
    {
        std::atomic<int> value;
    };

    // 分配计数器数组 (每个线程一个计数器)
    Counter *counters =
        static_cast<Counter *>(allocate_object(strategy, sizeof(Counter) * THREAD_COUNT));

    for (size_t i = 0; i < THREAD_COUNT; ++i)
    {
        new (&counters[i]) Counter();
    }

    std::latch latch(THREAD_COUNT);
    std::vector<std::thread> threads;

    auto worker = [&](size_t thread_id) {
        latch.arrive_and_wait();

        for (size_t i = 0; i < ITERS; ++i)
        {
            // 每个线程更新自己的计数器
            counters[thread_id].value.fetch_add(1, std::memory_order_relaxed);
        }
    };

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < THREAD_COUNT; ++i)
    {
        threads.emplace_back(worker, i);
    }

    for (auto &t : threads)
    {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "  伪共享测试: " << duration.count() << " μs\n";

    // 释放
    for (size_t i = 0; i < THREAD_COUNT; ++i)
    {
        counters[i].~Counter();
    }
    deallocate_object(counters, strategy);
}

int main()
{
    std::cout << "=== 内存对齐策略性能比较 ===" << std::endl;
    std::cout << "对象大小: " << OBJECT_SIZE << " 字节\n";
    std::cout << "对象数量: " << OBJECT_COUNT << "\n";
    std::cout << "迭代次数: " << ITERATIONS << "\n";
    std::cout << "硬件缓存行: " << std::hardware_destructive_interference_size
              << " 字节\n\n";

    // 测试三种策略
    test_performance(AlignmentStrategy::None, "无对齐");
    test_performance(AlignmentStrategy::CacheLine, "缓存行对齐");
    test_performance(AlignmentStrategy::ObjectSize, "对象大小对齐");

    std::cout << "\n=== 多线程伪共享测试 ===" << std::endl;
    test_false_sharing(AlignmentStrategy::None, "无对齐");
    test_false_sharing(AlignmentStrategy::CacheLine, "缓存行对齐");
    test_false_sharing(AlignmentStrategy::ObjectSize, "对象大小对齐");

    // NOTE: 测试结果每次都有波动. 说明没必要对齐
    // NOTE: 默认情况下使用自然对齐
    return 0;
}
// NOLINTEND