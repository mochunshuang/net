#include <array>
#include <iostream>

struct X
{
    X()
    {
        throw std::runtime_error("X(): std::runtime_error");
    }

    // custom placement new
    static void *operator new(std::size_t sz, bool b)
    {
        std::cout << "custom placement new called, b = " << b << '\n';
        return ::operator new(sz);
    }

    // custom placement delete
    static void operator delete(void *ptr, bool b)
    {
        std::cout << "custom placement delete called, b = " << b << '\n';
        ::operator delete(ptr);
    }
};

int main()
{
    try
    {
        [[maybe_unused]] X *p1 = new (true) X;
    }
    catch (const std::exception &ex)
    {
        std::cout << "exception: " << ex.what() << '\n';
    }
    {
        // sized class-specific deallocation functions
        struct X
        {
            static void operator delete(void *ptr, std::size_t sz)
            {
                std::cout << "custom delete for size " << sz << '\n';
                ::operator delete(ptr);
            }

            static void operator delete[](void *ptr, std::size_t sz)
            {
                std::cout << "custom delete for size " << sz << '\n';
                ::operator delete[](ptr);
            }
        };
        X *p1 = new X;
        delete p1;

        X *p2 = new X[10];
        delete[] p2;
        // custom delete for size 1
        // custom delete for size 18
        // NOTE: 数组是有额外的抽象的
        static_assert(sizeof(X) == 1);
        static_assert(sizeof(p1) == 8); // NOLINT
        static_assert(sizeof(p2) == 8); // NOLINT

        // NOTE: 不会走 X 的自定义 operator delete
        auto *ptr = new std::array<X, 10>;
        delete ptr;
    }
}
