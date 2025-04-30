#include <cassert>
#include <charconv>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

template <typename T>
struct parser_result
{
    std::optional<T> value; // NOLINT
    constexpr explicit operator bool() const noexcept
    {
        return value.has_value();
    }
};

// NOLINTBEGIN

// optional 可用作可能失败的工厂的返回类型
std::optional<std::string> create(bool b)
{
    if (b)
        return "Godzilla";
    return {};
}

// 能用 std::nullopt 创建任何（空的）std::optional
std::optional<std::string> create2(bool b)
{
    // NOTE: 崩溃: 是因为 外面的 int test2() 没设置返回值........
    return b ? std::optional<std::string>{"Godzilla"} : std::nullopt;
    // if (b)
    //     return std::string("Godzilla");
    // return std::nullopt;
}

int test2()
{
    std::cout << "create(false) 返回 " << create(false).value_or("empty") << '\n';
    // if (auto r = create(false))
    // {
    // }

    // 返回 optional 的工厂函数可用作 while 和 if 的条件

    if (auto str = create2(true))
    {
        std::cout << "create2(true) 返回 " << str.value() << '\n';
    }
    auto str = create2(true).value();
    std::cout << "create2(true) 返回 " << str << '\n';

    return 0;
}
std::optional<int> to_int(std::string_view sv)
{
    int r{};
    auto [ptr, ec]{std::from_chars(sv.data(), sv.data() + sv.size(), r)};
    if (ec == std::errc())
        return r;
    else
        return std::nullopt;
}
void test3()
{
    using namespace std::literals;

    std::vector<std::span<std::uint8_t>> results;

    const std::vector<std::optional<std::string>> v{
        "1234", "15 foo", "bar", "42", "5000000000", " 5", std::nullopt, "-43"};

    for (auto &&x : v | std::views::transform([](auto &&o) {
                        // 调试打印输入的 optional<string> 的内容
                        std::cout << std::left << std::setw(13)
                                  << std::quoted(o.value_or("nullopt")) << " -> ";

                        return o
                            // 若 optional 为 nullopt 则转换它为持有 "" 字符串的 optional
                            .or_else([] { return std::optional{""s}; })
                            // 拉平映射 string 为 int （失败时产生空的 optional）
                            .and_then(to_int)
                            // 映射 int 为 int + 1
                            .transform([](int n) { return n + 1; })
                            // 转换回 string
                            .transform([](int n) { return std::to_string(n); })
                            // 以 and_then 替换，并用 "NaN" 变换并忽略所有剩余的空
                            // optional and_then and ignored by transforms with "NaN"
                            .value_or("NaN"s);
                    }))
        std::cout << x << '\n';
}
// NOLINTEND
int main()
{
    constexpr parser_result<int> K_v; // NOLINT
    static_assert(not K_v);

    {
        parser_result<int> v;
        assert(not v);
        v.value = std::nullopt;
        assert(not v);
        v.value = 0;
        assert(v);

        static_assert(std::is_same_v<decltype(v.value.value()), int &>);
    }
    {
        std::optional<std::string> str = "Godzilla";
        if (str)
        {
            std::cout << "返回 " << *str << '\n';
        }
    }
    {
        test2();
        test3();
    }
    {
        struct none
        {
        };
        std::optional<none> v;
        assert(not v);
        auto ret = []() -> bool {
            return static_cast<bool>([] {
                return std::optional<none>{none{}};
            }());
        }();
        assert(ret == true);

        {
            auto fun = [] {
                return std::optional<none>{none{}};
            };
            // NOTE: 是不合法的
            //  auto fun2 = [&] {
            //      return (auto ret = fun()) ? std::optional<none>{none{}} :
            //      std::nullopt;
            //  };
            [[maybe_unused]] auto fun3 = [&] {
                auto ret = fun();
                return ret ? std::optional<none>{none{}} : std::nullopt;
            };
        }
    }

    std::cout << "main done\n";
    return 0;
}