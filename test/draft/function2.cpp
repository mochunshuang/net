#include <functional>
#include <iostream>
#include <vector>

// NOLINTBEGIN

int main()
{
    // std::function<void(auto &&...)> c; //NOTE: 不可能
    using T = decltype(+[](int a) noexcept {});

    T a = [](int a) noexcept {
        std::cout << "a: " << a << '\n';
    };
    a(2);
    // NOTE:
    auto regester = [](T fun) {
        fun(0);
    };
    regester([](int a) noexcept { std::cout << "regester fun: 0\n"; });
    regester([](int a) noexcept { std::cout << "regester fun: 1\n"; });

    struct A
    {
        static void fun(int a) noexcept
        {
            std::cout << "A::fun " << a << '\n';
        }

        static void fun2(int a, std::string s) noexcept
        {
            std::cout << "A::fun2 " << a << '\n';
        }
    };
    regester(&A::fun); // NOTE: OK

    // NOTE: 因此可以放到一个集合中
    auto reg =
        std::vector<T>{&A::fun, [](int a) noexcept { std::cout << "regester fun: 0\n"; },
                       [](int a) noexcept {
                           std::cout << "regester fun: 1\n";
                       }};
    reg.emplace_back([](int a) noexcept { std::cout << "regester fun: 2\n"; });
    for (auto &f : reg)
    {
        f(0);
    }
    {
        // NOTE: 那又如何?
        using T = decltype(+[](int a, std::vector<std::string> &&ags) noexcept {});
        auto reg = std::vector<T>{[](int a, std::vector<std::string> &&ags) noexcept {
            std::cout << "regester ags: ....\n";
            // TODO 如果调用 &A::fun2。// NOTE: 只能两次运行时强转
            void *f = (void *)&A::fun2; // NOTE: 统一存
            // NOTE: int a, std::string s 需要编译期构造。 function 萃取很简单
            auto *fun = (void (*)(int a, std::string s))(
                f); // NOTE: 统一取。 存取可以一个lambda + 模板封装
            fun(a, ags[0]);
        }};

        reg[0](1, std::vector<std::string>{"str"});
    }
    //

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND