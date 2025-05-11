#pragma once

#include <atomic>
#include <cassert>
#include <expected>
#include <source_location>
#include <span>
#include <string_view>
#include <iostream>

#include <sstream>
#include <utility>

#include "../../include/__detail/abnf/detail/__types.hpp"

namespace mcstest
{
    struct test_counter
    {
        std::atomic<std::size_t> pass_count;  // NOLINT
        std::atomic<std::size_t> total_count; // NOLINT

        void print() const noexcept
        {

            std::cout << "\033[32m"; // 开始绿色输出
            std::cout << "total_assert: " << total_count << ", ";
            std::cout << "pass_assert: " << pass_count << '\n';
            std::cout << "\033[0m"; // 重置颜色到默认设置
        }
    };

    static auto &get_test_counter() // NOLINT
    {
        static test_counter cunter;
        struct print // NOLINT
        {
            ~print() noexcept
            {
                cunter.print();
            }
        };
        static print p;
        return cunter;
    }

    struct test_info
    {
        std::string_view name;         // NOLINT
        std::source_location location; // NOLINT
    };

    class ExpectError : public std::exception
    {
      public:
        explicit ExpectError(std::string msg) : m_message(std::move(msg)) {}
        [[nodiscard]] const char *what() const noexcept override
        {
            return m_message.c_str();
        }

      private:
        std::string m_message;
    };

    // NOLINTNEXTLINE
    static std::string formatErrorMessage(const std::source_location &location) noexcept
    {
        std::stringstream ss;
        ss << "file_name: " << location.file_name() << ", line: " << location.line()
           << ", column: " << location.column();

        if (location.function_name() == nullptr)
        {
            ss << ", function_name: " << location.function_name();
        }
        ss << "\n";
        return ss.str();
    }

    auto expect(const bool &expr, // NOLINT
                const std::source_location &s = std::source_location::current()) -> void
    {
        if (expr)
            get_test_counter().pass_count++;
        else
        {
            throw ExpectError(formatErrorMessage(s));
        }
        get_test_counter().total_count++;
    }
    static auto unexpect( // NOLINT
        std::string_view msg,
        const std::source_location &s = std::source_location::current()) noexcept
    {
        std::cout << "unexpect: " << msg << " ,line: " << s.line();
        std::abort();
    }

    struct Test
    {
        test_info info; // NOLINT

        auto &operator=(auto &&test)
        {
            try
            {
                test();
            }
            catch (const ExpectError &e)
            {
                std::cout << "testname: " << info.name << " fault: " << e.what();
                throw;
            }
            return *this;
        }
    };

    // NOLINTNEXTLINE
    static auto add_test(
        std::string_view name,
        std::source_location location = std::source_location::current()) noexcept
    {
        return Test{.info = test_info{.name = name, .location = location}};
    }
}; // namespace mcstest

#define TEST(name) mcstest::add_test(name) // NOLINT
#define EXPECT(v) \
    mcstest::expect((v)) // NOLINT  ((v))确保传递的是一个完整的表达式而不是函数调用
#define UNEXPECT(message) mcstest::unexpect(message) // NOLINT

#define REQUIRE(v, message) \
    if ((v))                \
        EXPECT(v);          \
    else                    \
        UNEXPECT(message)

template <typename V, typename E>
constexpr auto HAS_VALUE(const std::expected<V, E> &e, const char *msg = "")
{
    if (e.has_value())
        EXPECT(true);
    else
        UNEXPECT(msg);
}

constexpr auto REQUIRE_TRUE(const bool &b, const char *msg = "")
{
    if (b)
        EXPECT(true);
    else
        UNEXPECT(msg);
}

constexpr auto REQUIRE_FALSE(const bool &b, const char *msg = "")
{
    REQUIRE_TRUE(not b, msg);
}

using OCTET = std::uint8_t;
template <size_t N>
consteval auto make_array(const char (&str)[N]) noexcept // NOLINT
{
    std::array<OCTET, N - 1> arr{};
    for (size_t i = 0; i < N - 1; ++i)
    {
        arr[i] = static_cast<OCTET>(str[i]);
    }
    return arr;
}

template <std::size_t N>
struct make_span
{
    using OCTET = std::uint8_t;
    OCTET octets[N - 1]{};                           // NOLINT
    constexpr make_span(char const (&s)[N]) noexcept // NOLINT
    {
        for (size_t i = 0; i < N - 1; ++i)
        {
            octets[i] = static_cast<OCTET>(s[i]);
        }
    };

    constexpr auto size() const noexcept
    {
        return N - 1;
    }
};

template <make_span S>
consteval decltype(auto) operator""_span()
{
    return std::span{S.octets, S.size()};
}

struct make_oct
{
    using OCTET = std::uint8_t;
    OCTET oct[1]{};                                 // NOLINT
    constexpr make_oct(char const (&s)[2]) noexcept // NOLINT
    {
        oct[0] = static_cast<OCTET>(s[0]);
    };
};

template <make_oct a>
consteval auto operator""_oct()
{
    return a.oct[0];
}

template <make_span S>
consteval decltype(auto) operator""_ctx()
{
    using mcs::abnf::detail::make_parser_ctx;
    return make_parser_ctx(std::span{S.octets, S.size()});
}

template <typename Rule>
constexpr auto make_pass_test() noexcept
{
    return [](const auto &array) constexpr {
        using mcs::abnf::detail::make_parser_ctx;
        using mcs::abnf::detail::parser_ctx;
        auto span = std::span{array};
        parser_ctx ctx = make_parser_ctx(span);
        assert(ctx.cur_index == 0);
        auto suc = Rule{}(ctx);
        assert(ctx.cur_index == span.size());
        assert(ctx.done());
        return suc;
    };
}

template <typename Rule>
constexpr auto make_unpass_test() noexcept
{
    return [](const auto &array) constexpr {
        using mcs::abnf::detail::make_parser_ctx;
        using mcs::abnf::detail::parser_ctx;
        auto span = std::span{array};
        parser_ctx ctx = make_parser_ctx(span);
        assert(ctx.cur_index == 0);
        auto suc = Rule{}(ctx);
        assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
        assert(not ctx.done());
        return suc;
    };
}

template <typename Rule>
constexpr auto make_rule_test(const auto &array) noexcept
{
    using mcs::abnf::detail::make_parser_ctx;
    using mcs::abnf::detail::parser_ctx;
    auto span = std::span{array};
    parser_ctx ctx = make_parser_ctx(span);
    assert(ctx.cur_index == 0);
    auto suc = Rule{}(ctx);
    return std::make_pair(suc, ctx);
}