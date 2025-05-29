#include <iostream>
#include <stdexcept>

struct A
{
    A()
    {
        std::cout << "A() \n";
        throw std::runtime_error{"err"};
    }
    A(const A &) = default;
    A(A &&) = delete;
    A &operator=(const A &) = default;
    A &operator=(A &&) = delete;
    ~A()
    {
        std::cout << "~A() \n";
    }
};

int main()
{
    // NOTE: A异常后 不会调用 析构函数
    try
    {
        A a{};
    }
    catch (...)
    {
        std::cout << "A() 构造异常不会调用 ~A() 。因此 构造函数必须自己处理申请的资源 \n";
    }
    std::cout << "main done\n";
    return 0;
}