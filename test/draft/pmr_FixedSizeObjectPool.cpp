#include <memory_resource>
#include <vector>
#include <atomic>
#include <stdexcept>
#include <cstddef>

#include <iostream>

// NOLINTBEGIN

template <typename T, size_t MaxObjects>
class FixedSizeObjectPool
{
  public:
    // 构造函数：使用成员初始化列表正确初始化资源
    FixedSizeObjectPool()
        : buffer_(MaxObjects * sizeof(T)),
          bufferResource_(buffer_.data(), buffer_.size(),
                          std::pmr::get_default_resource()),
          poolResource_(std::pmr::pool_options{.max_blocks_per_chunk = MaxObjects,
                                               .largest_required_pool_block = sizeof(T)},
                        &bufferResource_)
    {
    }

    ~FixedSizeObjectPool()
    {
        // 确保所有对象都被释放
        if (allocatedCount_ != 0)
        {
            // 这里可以记录日志或抛出异常
        }
    }

    // 显式禁用拷贝构造和赋值
    FixedSizeObjectPool(const FixedSizeObjectPool &) = delete;
    FixedSizeObjectPool &operator=(const FixedSizeObjectPool &) = delete;

    // 移动构造函数和移动赋值运算符
    FixedSizeObjectPool(FixedSizeObjectPool &&) = delete;
    FixedSizeObjectPool &operator=(FixedSizeObjectPool &&) = delete;

    // 分配对象
    template <typename... Args>
    T *allocate(Args &&...args)
    {
        if (allocatedCount_ >= MaxObjects)
        {
            throw std::runtime_error("Object pool is full");
        }

        // 使用内存池资源的分配器
        std::pmr::polymorphic_allocator<T> alloc(&poolResource_);

        // 分配内存并构造对象
        T *obj = alloc.allocate(1);
        try
        {
            alloc.construct(obj, std::forward<Args>(args)...);
            ++allocatedCount_;
            return obj;
        }
        catch (...)
        {
            alloc.deallocate(obj, 1);
            throw;
        }
    }

    // 回收对象
    void deallocate(T *obj)
    {
        if (obj == nullptr)
            return;

        // 使用相同的分配器
        std::pmr::polymorphic_allocator<T> alloc(&poolResource_);

        // 析构并释放对象
        alloc.destroy(obj);
        alloc.deallocate(obj, 1);
        --allocatedCount_;
    }

    // 获取当前已分配对象数量
    size_t size() const noexcept
    {
        return allocatedCount_;
    }

    // 获取最大容量
    size_t capacity() const noexcept
    {
        return MaxObjects;
    }

    // 判断对象池是否已满
    bool full() const noexcept
    {
        return allocatedCount_ >= MaxObjects;
    }

  private:
    std::vector<std::byte> buffer_;                       // 预分配的内存缓冲区
    std::pmr::monotonic_buffer_resource bufferResource_;  // 基于缓冲区的内存资源
    std::pmr::unsynchronized_pool_resource poolResource_; // 固定大小的内存池资源
    std::atomic<size_t> allocatedCount_{0};               // 当前已分配对象数量
};

// 测试类
class MyClass
{
  public:
    MyClass(int value) : data(value)
    {
        std::cout << "Constructing MyClass with value: " << data << std::endl;
    }

    ~MyClass()
    {
        std::cout << "Destructing MyClass with value: " << data << std::endl;
    }

    void doSomething() const
    {
        std::cout << "Doing something with value: " << data << std::endl;
    }

  private:
    int data;
};

int main()
{
    // 创建一个最多容纳10个MyClass对象的对象池
    FixedSizeObjectPool<MyClass, 10> pool;

    // 从对象池分配对象
    MyClass *obj1 = pool.allocate(42);
    MyClass *obj2 = pool.allocate(99);

    // 使用对象
    obj1->doSomething();
    obj2->doSomething();

    // 释放对象回对象池
    pool.deallocate(obj1);
    pool.deallocate(obj2);

    // 对象池会在其生命周期结束时自动释放所有内存

    return 0;
}
// NOLINTEND