#include <iostream>
#include <memory>
#include <string>

// NOLINTBEGIN

class LazyObject
{
    std::string name;

  public:
    LazyObject()
    {
        std::cout << "Default constructed\n";
    }
    explicit LazyObject(std::string n) : name(std::move(n))
    {
        std::cout << "Constructed " << name << "\n";
    }
    ~LazyObject()
    {
        std::cout << "Destroyed " << name << "\n";
    }
    void setName(std::string n)
    {
        name = std::move(n);
    }
    void greet() const
    {
        if (!name.empty())
        {
            std::cout << "Hello, I'm " << name << "\n";
        }
        else
        {
            std::cout << "I'm [an unnamed object]\n";
        }
    }
};

int main()
{
    // 分配未初始化的内存
    std::cout << "========allocate start" << "\n";
    auto p = std::allocator<LazyObject>().allocate(3);
    std::cout << "========allocate end" << "\n";

    // 默认构造
    std::cout << "========uninitialized_default_construct_n start" << "\n";
    std::uninitialized_default_construct_n(p, 3);
    std::cout << "========uninitialized_default_construct_n end" << "\n";

    // NOTE: uninitialized_default_construct_n执行默认构造，适用于需要延迟初始化的场景
    //  延迟初始化
    p[0].setName("Alice");
    p[1].setName("Bob");
    // p[2]保持默认构造状态

    for (int i = 0; i < 3; ++i)
    {
        p[i].greet();
    }

    // 销毁
    std::cout << "========destroy_n start" << "\n";
    std::destroy_n(p, 3);
    std::cout << "========destroy_n end" << "\n";

    // 释放内存
    std::allocator<LazyObject>().deallocate(p, 3);
}
// NOLINTEND