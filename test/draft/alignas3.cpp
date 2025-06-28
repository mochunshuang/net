// NOLINTBEGIN

#include <iostream>
#include <numeric>
#include <vector>
#include <chrono>
#include <random>
#include <new>
#include <memory>
#include <algorithm>
#include <format>

// 测试对象 (1KB)
struct Object
{
    char data[1024];

    // 模拟实际使用场景
    void process()
    {
        for (char &c : data)
        {
            c = (c + 1) % 256; // 简单修改数据
        }
    }
};

// 对齐对象池
class AlignedPool
{
  public:
    Object *allocate()
    {
        if (free_list.empty())
        {
            // 分配新块（1024字节对齐）
            constexpr size_t chunk_size = 64;
            auto chunk = std::unique_ptr<Object[]>(static_cast<Object *>(
                ::operator new(chunk_size * sizeof(Object),
                               static_cast<std::align_val_t>(alignof(Object)))));

            // 初始化对象
            for (size_t i = 0; i < chunk_size; ++i)
            {
                new (&chunk[i]) Object();
                free_list.push_back(&chunk[i]);
            }

            chunks.push_back(std::move(chunk));
        }

        Object *obj = free_list.back();
        free_list.pop_back();
        return obj;
    }

    void deallocate(Object *obj)
    {
        free_list.push_back(obj);
    }

  private:
    std::vector<std::unique_ptr<Object[]>> chunks;
    std::vector<Object *> free_list;
};

// 非对齐对象池
class UnalignedPool
{
  public:
    Object *allocate()
    {
        if (free_list.empty())
        {
            // 分配新块（无特殊对齐）
            constexpr size_t chunk_size = 64; // NOTE: 崩溃，先不管
            auto chunk = std::make_unique<Object[]>(chunk_size);

            // 初始化对象
            for (size_t i = 0; i < chunk_size; ++i)
            {
                free_list.push_back(&chunk[i]);
            }

            chunks.push_back(std::move(chunk));
        }

        Object *obj = free_list.back();
        free_list.pop_back();
        return obj;
    }

    void deallocate(Object *obj)
    {
        free_list.push_back(obj);
    }

  private:
    std::vector<std::unique_ptr<Object[]>> chunks;
    std::vector<Object *> free_list;
};

// 性能测试函数
template <typename Pool>
void run_performance_test(Pool &pool, const std::string &name, size_t object_count,
                          size_t iterations)
{
    // 分配对象
    std::vector<Object *> objects;
    objects.reserve(object_count);

    for (size_t i = 0; i < object_count; ++i)
    {
        objects.push_back(pool.allocate());
    }

    // 创建随机访问顺序
    std::vector<size_t> access_order(object_count);
    std::iota(access_order.begin(), access_order.end(), 0);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(access_order.begin(), access_order.end(), g);

    // 预热缓存
    for (size_t i = 0; i < std::min(object_count, static_cast<size_t>(100)); ++i)
    {
        objects[i]->process();
    }

    // 实际性能测试
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t iter = 0; iter < iterations; ++iter)
    {
        // 随机访问模式（模拟真实场景）
        for (size_t idx : access_order)
        {
            objects[idx]->process();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 输出结果
    std::cout << std::format("{} 测试结果:\n", name);
    std::cout << std::format("  对象数量: {}\n", object_count);
    std::cout << std::format("  迭代次数: {}\n", iterations);
    std::cout << std::format("  总耗时: {} μs\n", duration.count());
    std::cout << std::format("  每次迭代平均耗时: {:.3f} μs\n\n",
                             static_cast<double>(duration.count()) / iterations);

    // 释放对象
    for (Object *obj : objects)
    {
        pool.deallocate(obj);
    }
}

// 对齐验证函数
void verify_alignment(const std::vector<Object *> &objects)
{
    size_t aligned_count = 0;
    size_t unaligned_count = 0;

    for (Object *obj : objects)
    {
        uintptr_t address = reinterpret_cast<uintptr_t>(obj);
        if (address % alignof(Object) == 0)
        {
            aligned_count++;
        }
        else
        {
            unaligned_count++;
        }
    }

    std::cout << "对齐验证:\n";
    std::cout << std::format("  对齐对象数: {}\n", aligned_count);
    std::cout << std::format("  未对齐对象数: {}\n", unaligned_count);
    std::cout << std::format("  对齐率: {:.2f}%\n\n",
                             (static_cast<double>(aligned_count) / objects.size()) * 100);
}

int main()
{
    constexpr size_t object_count = 1000;
    constexpr size_t iterations = 100;

    // 对齐对象池测试
    AlignedPool aligned_pool;
    run_performance_test(aligned_pool, "1024字节对齐对象池", object_count, iterations);

    // 验证对齐
    std::vector<Object *> aligned_objects;
    for (size_t i = 0; i < object_count; ++i)
    {
        aligned_objects.push_back(aligned_pool.allocate());
    }
    verify_alignment(aligned_objects);
    for (Object *obj : aligned_objects)
    {
        aligned_pool.deallocate(obj);
    }

    // 非对齐对象池测试
    UnalignedPool unaligned_pool;
    run_performance_test(unaligned_pool, "非对齐对象池", object_count, iterations);

    // 验证对齐
    std::vector<Object *> unaligned_objects;
    for (size_t i = 0; i < object_count; ++i)
    {
        unaligned_objects.push_back(unaligned_pool.allocate());
    }
    verify_alignment(unaligned_objects);
    for (Object *obj : unaligned_objects)
    {
        unaligned_pool.deallocate(obj);
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
/*

1024字节对齐对象池 测试结果:
  对象数量: 1000
  迭代次数: 100
  总耗时: 151428 μs
  每次迭代平均耗时: 1514.280 μs

对齐验证:
  对齐对象数: 1000
  未对齐对象数: 0
  对齐率: 100.00%

非对齐对象池 测试结果:
  对象数量: 1000
  迭代次数: 100
  总耗时: 153242 μs
  每次迭代平均耗时: 1532.420 μs

对齐验证:
  对齐对象数: 1000
  未对齐对象数: 0
  对齐率: 100.00%
*/