#include <iostream>
#include <memory>
#include <memory_resource>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <string>
// NOLINTBEGIN

// 测试配置
constexpr size_t ALLOCATION_COUNT = 1000000;
constexpr size_t OBJECT_SIZE = 64; // 字节

// 测试对象类型
struct alignas(16) TestObject
{
    alignas(8) char data[OBJECT_SIZE];
};

// 计时器类
class Timer
{
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point start_;

  public:
    Timer() : start_(Clock::now()) {}
    double elapsed() const
    {
        return std::chrono::duration<double, std::milli>(Clock::now() - start_).count();
    }
};

// 测试函数模板
template <typename Allocator>
double testAllocation(const std::string &name)
{
    std::vector<typename std::allocator_traits<Allocator>::pointer> pointers;
    pointers.reserve(ALLOCATION_COUNT);

    Allocator alloc;
    auto allocTraits =
        typename std::allocator_traits<Allocator>::template rebind_alloc<TestObject>(
            alloc);

    Timer timer;

    // 分配
    for (size_t i = 0; i < ALLOCATION_COUNT; ++i)
    {
        auto ptr = allocTraits.allocate(1);
        // 修复：使用allocator_traits的静态方法
        std::allocator_traits<decltype(allocTraits)>::construct(allocTraits, ptr);
        pointers.push_back(ptr);
    }

    // 随机打乱释放顺序
    std::shuffle(pointers.begin(), pointers.end(), std::mt19937{std::random_device{}()});

    // 释放
    for (auto ptr : pointers)
    {
        // 修复：使用allocator_traits的静态方法
        std::allocator_traits<decltype(allocTraits)>::destroy(allocTraits, ptr);
        allocTraits.deallocate(ptr, 1);
    }

    double time = timer.elapsed();
    std::cout << name << " 耗时: " << time << " ms\n";
    return time;
}

// 批量测试函数模板
template <typename Allocator>
double testBulkAllocation(const std::string &name)
{
    Allocator alloc;
    auto allocTraits =
        typename std::allocator_traits<Allocator>::template rebind_alloc<TestObject>(
            alloc);

    Timer timer;

    // 批量分配
    auto ptr = allocTraits.allocate(ALLOCATION_COUNT);

    // 批量构造
    for (size_t i = 0; i < ALLOCATION_COUNT; ++i)
    {
        // 修复：使用allocator_traits的静态方法
        std::allocator_traits<decltype(allocTraits)>::construct(allocTraits, ptr + i);
    }

    // 批量销毁
    for (size_t i = 0; i < ALLOCATION_COUNT; ++i)
    {
        // 修复：使用allocator_traits的静态方法
        std::allocator_traits<decltype(allocTraits)>::destroy(allocTraits, ptr + i);
    }

    // 批量释放
    allocTraits.deallocate(ptr, ALLOCATION_COUNT);

    double time = timer.elapsed();
    std::cout << name << " 批量操作耗时: " << time << " ms\n";
    return time;
}

int main()
{
    std::cout << "测试配置: " << ALLOCATION_COUNT << " 次分配, 对象大小: " << OBJECT_SIZE
              << " 字节\n\n";

    // 测试 std::allocator
    std::cout << "=== 测试 std::allocator ===\n";
    double stdAllocTime = testAllocation<std::allocator<TestObject>>("std::allocator");
    double stdAllocBulkTime =
        testBulkAllocation<std::allocator<TestObject>>("std::allocator");

    // 测试 polymorphic_allocator + 默认内存资源
    std::cout << "\n=== 测试 polymorphic_allocator + 默认内存资源 ===\n";
    std::pmr::polymorphic_allocator<TestObject> polyAllocDefault;
    double polyAllocDefaultTime =
        testAllocation<decltype(polyAllocDefault)>("polymorphic_allocator (默认)");
    testBulkAllocation<decltype(polyAllocDefault)>("polymorphic_allocator (默认)");

    // 测试 polymorphic_allocator + 池式内存资源
    std::cout << "\n=== 测试 polymorphic_allocator + 池式内存资源 ===\n";
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::polymorphic_allocator<TestObject> polyAllocPool(&pool);
    double polyAllocPoolTime =
        testAllocation<decltype(polyAllocPool)>("polymorphic_allocator (池式)");
    double polyAllocPoolBulkTime =
        testBulkAllocation<decltype(polyAllocPool)>("polymorphic_allocator (池式)");

    // 输出相对性能
    std::cout << "\n=== 相对性能 ===\n";
    std::cout << "polymorphic_allocator (默认) 速度是 std::allocator 的 "
              << stdAllocTime / polyAllocDefaultTime * 100 << "%\n";
    std::cout << "polymorphic_allocator (池式) 速度是 std::allocator 的 "
              << stdAllocTime / polyAllocPoolTime * 100 << "%\n";
    std::cout << "polymorphic_allocator (池式) 批量操作速度是 std::allocator 的 "
              << stdAllocBulkTime / polyAllocPoolBulkTime * 100 << "%\n";

    // NOTE: std::allocator 更优
    return 0;
}
// NOLINTEND