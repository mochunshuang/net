
// NOLINTBEGIN
#include <cassert>
#include <expected>
#include <string_view>
struct Base
{
    virtual constexpr int get() const
    {
        return 1;
    }
};
struct Derived : Base
{
    constexpr int get() const override
    {
        return 2;
    }
};

consteval int test()
{
    Derived d;
    const Base &b = d;
    return b.get(); // 编译期多态，返回 2
}

#include <iostream>

struct BaseException
{
    virtual constexpr const char *what() const noexcept = 0;
};
struct DerivedException : BaseException
{
    constexpr const char *what() const noexcept override
    {
        return "Derived";
    }
};

consteval int compileTimeCalc()
{
    try
    {
        throw DerivedException{}; // 编译期抛出多态异常
    }
    catch (const BaseException &e)
    {                       // 多态捕获
        return e.what()[0]; // 返回 'D' 的 ASCII 码
    }
}
// static_assert(compileTimeCalc() == 68); // 不通过，c++26 一个可以了

enum class ErrorCode
{
    Ok,
    InvalidInput,
    RuntimeFailure
};

constexpr std::expected<int, ErrorCode> compute(int v)
{
    if (v < 0)
        return std::unexpected(ErrorCode::InvalidInput);

    return 1;
}

int main()
{
    static_assert(test() == 2); // 通过
    {
        constexpr DerivedException d;
        static_assert(std::string_view(d.what()) == "Derived");
    }
    {
        DerivedException d;
        assert(std::string_view(d.what()) == "Derived");
    }
    {
        try
        {
            throw DerivedException{}; // 编译期抛出多态异常
        }
        catch (const BaseException &e)
        { // 多态捕获
            std::cout << "DerivedException: " << std::string_view(e.what()) << '\n';
        }
    }
    {
        constexpr auto v = compute(1);
        static_assert(v.value() == 1);

        constexpr auto e = compute(-1);
        static_assert(e.error() == ErrorCode::InvalidInput);
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND