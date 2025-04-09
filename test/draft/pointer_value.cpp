#include <cassert>
#include <iostream>

// NOLINTBEGIN
int main()
{
    // 单继承且父类无虚函数
    {
        struct Base
        {
            int x;
        };
        struct Derived : Base
        {
            int y;
        };
        Derived d{};
        Base *b = &d; // b的地址等于&d，指针值不变
        auto b2 = b;
        assert(b2 == b); // 指针是值传递
        assert(b2 == &d);
    }
    {
        // 单继承且父类有虚函数
        struct Base
        {
            int x;
        };
        struct Derived : Base
        {
            int y;
            virtual void foo() {}
        };
        Derived d;
        Base *b = &d; // b的地址仍等于&d
        auto b2 = b;
        assert(b2 == b); // 指针是值传递
        assert(b2 == &d);

        assert((void *)b != (void *)&d);
        assert((void *)b2 != (void *)&d);
    }
    {
        // 指针值可能变化：当派生类继承多个基类时，转换为不同基类指针可能需要调整地址。
        struct Base1
        {
            int x;
        };
        struct Base2
        {
            int y;
        };
        struct Derived : Base1, Base2
        {
        };
        Derived d;
        Base1 *b1 = &d; // 地址等于&d
        Base2 *b2 = &d; // 地址可能为&d + sizeof(Base1)

        assert((void *)b2 != (void *)b1);

        assert(b1 == &d); // 指针是值传递
        assert(b2 == &d); // 指针是值传递

        assert((void *)b1 == &d); // 指针是值传递
        assert((void *)b2 != &d); // 指针是值传递
    }
    {
        // 虚继承（Virtual Inheritance）
        struct Base
        {
            int x;
        };
        struct Derived1 : virtual Base
        {
        };
        struct Derived2 : virtual Base
        {
        };
        struct Final : Derived1, Derived2
        {
        };
        Final f;
        Base *b = &f; // 地址可能不等于&f，需通过虚基类表调整
        assert((void *)b != &f);
    }
    {
        struct Base1
        {
            virtual ~Base1() {}
            int x;
        };
        struct Base2
        {
            int y;
        };
        struct Derived : Base1, Base2
        {
        };
        Derived d;
        Derived *source = &d;
        Base1 *b1 = static_cast<Base1 *>(source);
        Base2 *b2 = static_cast<Base2 *>(source);
        std::cout << "源地址: " << source << '\n'
                  << "Base1*: " << b1 << '\n'
                  << "Base2*: " << b2 << '\n';
        assert((void *)source != b2);
    }
    {
        // 在单继承体,无虚函数，基类指针的值永远等于派生类对象的源地址
        class A
        {
            int a{};
        };
        class B : public A
        {
            float b{};
        };
        class C : public B
        {
            double c{};
        };
        class D : public C
        {
            std::string d{};
        };

        D d;
        C *pC = &d; // pC 的值 == &d
        assert((void *)pC == (void *)&d);
        {
            D *pD = &d;
            B *pC = static_cast<B *>(pD);
            C *pB = static_cast<C *>(pC);
            A *pA = static_cast<A *>(pB);

            assert((void *)pC == (void *)&d);

            // 值传递都是同一个地址，不做地址调整
            assert((void *)pC == (void *)(pD));
            assert((void *)pC == (void *)(pB));
            assert((void *)pC == (void *)(pA));

            // NOTE: 指针隐式转换,更优雅
            auto fun = [](A *) {
            };
            fun(pB);
            fun(pC);
            fun(&d);
        }
    }
    return 0;
}
// NOLINTEND