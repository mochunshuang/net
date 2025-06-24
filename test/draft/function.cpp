#include <iostream>
#include <string_view>

// NOLINTBEGIN

template <auto &f>
struct fun
{
    static constexpr auto invoke = f; // NOLINT
};

static void fun0(int a) noexcept
{
    std::cout << "fun0: " << a << '\n';
}

static void fun1(int a, double b) noexcept
{
    std::cout << "fun1: " << a << "," << b << '\n';
}

struct A
{
    static void fun1(int a, double b) noexcept
    {
        std::cout << "A::fun1: " << a << "," << b << '\n';
    }
};

template <auto &f>
struct str
{
    static constexpr auto string = f;
};

template <size_t N>
struct FixedString
{
    static constexpr size_t size = N;                    // NOLINT
    char value[N]{};                                     // NOLINT
    constexpr FixedString(const char (&str)[N]) noexcept // NOLINT
    {
        std::copy_n(str, N, value);
    }
};

template <FixedString s>
struct name
{
    static constexpr auto string = std::string_view{s.value};
};

int main()
{
    fun<fun0>::invoke(1);
    fun<fun1>::invoke(1, 2);
    fun<A::fun1>::invoke(1, 2);
    // NOTE: 类型擦除。 统一？
    {
        static constexpr std::string_view s = "abc"; // NOTE: 必须 static
        static_assert(str<s>::string == s);
        // static_assert(str<"abc">::string == s); //NOTE: 错误

        static_assert(name<"abc">::string == s);
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND