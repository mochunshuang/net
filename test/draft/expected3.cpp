#include <concepts>
#include <expected>
#include <iostream>
#include <string>
#include <type_traits>

// NOLINTBEGIN

template <typename T>
concept ValidResult =
    requires { requires std::same_as<T, std::expected<std::string, int>>; };

auto test(int v) -> std::expected<std::string, int>
{
    if (v == 0)
    {
        return std::unexpected(v); // 错误路径
    }
    return std::string{"1223"}; // 正确路径
}

// 编译错误
auto test2(int v) -> ValidResult auto
{
    if (v == 0)
    {
        return std::expected<std::string, int>(std::unexpected(v)); // 显式构造
    }
    return std::expected<std::string, int>("1223"); // 显式构造
}

namespace __detail
{
    template <typename T>
    inline constexpr bool is_expected = false;
    template <typename V, typename E>
    inline constexpr bool is_expected<std::expected<V, E>> = true;
}; // namespace __detail

template <typename T>
concept is_expected_specialization = __detail::is_expected<T>;

template <typename R>
concept abnf_result = is_expected_specialization<R> || std::same_as<R, bool>;

auto valid_func(int x) -> abnf_result auto
{
    if (x < 0)
    {
        return std::expected<std::string, int>(std::unexpected(x));
    }
    return std::expected<std::string, int>("Value: " + std::to_string(x));
}

enum A
{
    a,
    b
};

auto valid_func2(int x) -> abnf_result auto
{
    if (x < 0)
    {
        return std::expected<std::string, A>(std::unexpected(A::a));
    }
    return std::expected<std::string, A>("Value: " + std::to_string(x));
}

template <typename V, typename E = A>
struct result_builder
{
    using value_type = V;
    using error_type = E;
    using result_type = std::expected<value_type, error_type>;

    template <typename... Args>
    inline static constexpr auto success(Args &&...args) noexcept
    {
        return result_type(std::in_place, std::forward<Args>(args)...);
    }

    template <typename Err>
    inline static constexpr auto fail(Err &&err) noexcept
    {
        return result_type(std::unexpected(std::forward<Err>(err)));
    }
};

auto valid_func3(int x) -> abnf_result auto
{
    using builder = result_builder<std::string>;
    if (x < 0)
    {
        return builder::fail(A::a); // 错误路径
    }
    return builder::success("Value: " + std::to_string(x)); // 成功路径
}

template <typename T>
struct my_type
{
    explicit my_type(const T &) noexcept {}
};

auto valid_func4(int x) -> abnf_result auto
{
    using Builder = result_builder<my_type<std::string>>;
    if (x < 0)
    {
        return Builder::fail(A::b); // 错误路径
    }
    return Builder::success("Value: " + std::to_string(x)); // 成功路径
}

abnf_result auto valid_func5(int x)
{
    return true;
}

int main()
{
    auto r1 = test(0);
    if (!r1)
        std::cout << "Error: " << r1.error() << "\n";

    {
        auto r1 = valid_func4(-1);
        if (!r1)
            std::cout << "Error: " << r1.error() << "\n";
    }

    return 0;
}
// NOLINTEND