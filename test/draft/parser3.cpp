#include <iostream>

// NOLINTBEGIN

namespace Counter
{
    // 前向声明函数对象类型
    struct Test;
    struct Test1;

    // 定义函数对象类型
    struct Test
    {
        constexpr int operator()(int a) const;
    };

    struct Test1
    {
        constexpr int operator()(int a) const;
    };

    // 实现相互递归
    constexpr int Test::operator()(int a) const
    {
        return a <= 0 ? 0 : Test1{}(a - 1);
    }

    constexpr int Test1::operator()(int a) const
    {
        return a <= 0 ? 0 : Test{}(a - 1);
    }
} // namespace Counter

int main()
{
    static_assert(Counter::Test{}(5) == 0); // 编译期验证
    std::cout << "运行时结果: " << Counter::Test{}(5) << std::endl;
    return 0;
}
// NOLINTEND