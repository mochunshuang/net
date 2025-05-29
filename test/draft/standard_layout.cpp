#include <cassert>
#include <iostream>
#include <type_traits>

struct Base
{
};
struct Derived : Base
{
};

int main()
{
    Derived d;
    Derived *pDerived = &d;

    // 错误：不能直接将 Derived* 转为 Base**
    // Base** pBasePtr = static_cast<Base**>(pDerived);  // 编译失败

    // 正确方式：先转 void*，再转 Base**
    void *pVoid = static_cast<void *>(&pDerived);
    Base **pBasePtr = static_cast<Base **>(pVoid);
    {
        auto *ptr = static_cast<Base **>(static_cast<void *>(&pDerived));
        assert((void *)pBasePtr == ptr);
        auto *p = reinterpret_cast<Base **>(&pDerived);
        assert(p == ptr);
    }

    // 值语言相同
    assert((void *)pBasePtr == (void *)&pDerived);

    // 验证单继承无虚函数时，转换是安全的
    static_assert(std::is_standard_layout_v<Derived>, "Derived must be standard-layout");
    static_assert(sizeof(Derived *) == sizeof(Base *),
                  "Pointers must have the same size");

    std::cout << "Conversion succeeded." << '\n';

    {
        // NOTE: 不能在父类和子类同时拥有非静态成员时保持标准布局。
        {
            struct Base
            {
            }; // 空类（无成员）

            struct Derived : Base
            {
                int x;
            };
            static_assert(std::is_standard_layout_v<Derived>, "OK");
        }
        {
            struct Base
            {
                int x;
            };

            struct Derived : Base
            {
            };
            static_assert(std::is_standard_layout_v<Derived>, "OK");
        }
        {
            struct Base
            {
                int a;
            };
            struct Derived : Base
            {
                int b;
            };

            static_assert(!std::is_standard_layout_v<Derived>, "Fail");
        }
        {
            struct Base
            {
                int a;
            };

            // 用组合代替继承
            struct Derived
            {
                Base base; // 组合
                int b;
            };
            // 满足标准布局（所有成员在同一类中）
            static_assert(std::is_standard_layout_v<Derived>, "OK");
        }
    }
    return 0;
}