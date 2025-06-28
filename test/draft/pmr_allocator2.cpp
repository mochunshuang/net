#include <iostream>
#include <memory>
#include <memory_resource>
#include <string>

// NOLINTBEGIN

// 一个复杂对象：包含资源管理和自定义构造函数
class ComplexObject
{
  private:
    std::string *data_;
    int *counter_; // 引用计数器

  public:
    // 带参数的构造函数
    ComplexObject(const std::string &value)
        : data_(new std::string(value)), counter_(new int(1))
    {
    }

    // 拷贝构造函数
    ComplexObject(const ComplexObject &other)
        : data_(other.data_), counter_(other.counter_)
    {
        ++(*counter_);
    }

    // 移动构造函数
    ComplexObject(ComplexObject &&other) noexcept
        : data_(other.data_), counter_(other.counter_)
    {
        other.data_ = nullptr;
        other.counter_ = nullptr;
    }

    // 析构函数
    ~ComplexObject()
    {
        if (counter_)
        {
            --(*counter_);
            if (*counter_ == 0)
            {
                delete data_;
                delete counter_;
            }
        }
    }

    // 禁止赋值运算符
    ComplexObject &operator=(const ComplexObject &) = delete;
    ComplexObject &operator=(ComplexObject &&) = delete;

    // 访问数据
    const std::string &value() const
    {
        return *data_;
    }
};

// 通用分配和构造函数
template <typename Allocator>
void allocateAndConstruct(Allocator alloc)
{
    using Traits = std::allocator_traits<Allocator>;
    using ValueType = typename Traits::value_type;

    // 分配内存
    ValueType *ptr = Traits::allocate(alloc, 1);

    try
    {
        // 使用allocator_traits构造对象（带参数）
        Traits::construct(alloc, ptr, "Hello, World!");

        // 使用对象
        std::cout << "Constructed object value: " << ptr->value() << "\n";

        // 使用allocator_traits销毁对象
        Traits::destroy(alloc, ptr);
    }
    catch (...)
    {
        // 异常处理：释放内存
        Traits::deallocate(alloc, ptr, 1);
        throw;
    }

    // 释放内存
    Traits::deallocate(alloc, ptr, 1);
}

int main()
{
    // 使用std::allocator
    std::cout << "Using std::allocator:\n";
    allocateAndConstruct<std::allocator<ComplexObject>>(std::allocator<ComplexObject>{});

    // 使用polymorphic_allocator
    std::cout << "\nUsing polymorphic_allocator:\n";
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::polymorphic_allocator<ComplexObject> polyAlloc(&pool);
    allocateAndConstruct(polyAlloc);

    return 0;
}
// NOLINTEND