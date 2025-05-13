#pragma once

#include <cstdint>
#include <span>
#include <concepts>
#include <type_traits>
#include <utility>
#include <array>

namespace mcs::common::ABNF
{

    using octet = std::uint8_t;

    // 解析结果类型
    struct ParseResult
    {
        bool success;    // NOLINT
        size_t consumed; // NOLINT

        constexpr explicit operator bool() const
        {
            return success;
        }
        constexpr bool operator!() const
        {
            return !success;
        }
    };

    // 解析器概念
    template <typename T>
    concept parser =
        (std::copy_constructible<T> || std::move_constructible<T>) &&
        std::destructible<T> && requires(T p, const std::span<const octet> &s) {
            { p(s) } noexcept -> std::same_as<ParseResult>;
        };

    // 基础解析器包装类
    template <parser P>
    struct Rule
    {
        P parser; // NOLINT

        constexpr explicit Rule(P &&p) noexcept : parser(std::forward<P>(p)) {} // NOLINT

        constexpr ParseResult operator()(const std::span<const octet> &s) const noexcept
        {
            return parser(s);
        }
    };
    template <typename P>
    Rule(P &&p) -> Rule<std::decay_t<P>>; // NOLINT

    // 基本字符解析器工厂
    template <typename Check>
    constexpr static auto make_char_rule(Check &&check) noexcept // NOLINT
    {
        return Rule{
            [check_ = std::forward<Check>(check)](
                const std::span<const octet> &s) constexpr noexcept -> ParseResult {
                return {!s.empty() && check_(s[0]),
                        static_cast<size_t>(s.empty() ? 0 : 1)};
            }};
    }

    // 运算符重载实现组合语法
    template <parser P1, parser P2> // NOLINTNEXTLINE
    constexpr static auto operator||(const Rule<P1> &lhs, const Rule<P2> &rhs) noexcept
    {
        return Rule{
            [=](const std::span<const octet> &s) constexpr noexcept -> ParseResult {
                auto r1 = lhs.parser(s);
                if (r1.success)
                    return r1;
                return rhs.parser(s);
            }};
    }

    template <parser P1, parser P2> // NOLINTNEXTLINE
    constexpr auto operator+(const Rule<P1> &lhs, const Rule<P2> &rhs) noexcept
    {
        return Rule{[=](std::span<const octet> s) noexcept -> ParseResult {
            auto r1 = lhs.parser(s);
            if (!r1.success)
                return {.success = false, .consumed = 0};

            auto r2 = rhs.parser(s.subspan(r1.consumed));
            if (!r2.success)
                return {.success = false, .consumed = 0};

            return {true, r1.consumed + r2.consumed};
        }};
    }

    // 重复组合器
    constexpr auto zero_or_more(parser auto p)
    {
        return Rule{[=](std::span<const octet> s) noexcept -> ParseResult {
            size_t total = 0;
            while (true)
            {
                auto result = p(s.subspan(total));
                if (!result.success || result.consumed == 0)
                    break;
                total += result.consumed;
            }
            return {.success = true, .consumed = total};
        }};
    }

    // Core Rules 实现
    inline constexpr auto ALPHA = make_char_rule([](const octet &c) constexpr noexcept {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    });

    constexpr auto BIT = make_char_rule(
        [](const octet &c) constexpr noexcept { return c == '0' || c == '1'; });

    constexpr auto DIGIT = make_char_rule([](octet c) { return c >= '0' && c <= '9'; });

    constexpr auto HEXDIG =
        DIGIT || make_char_rule([](octet c) { return c >= 'A' && c <= 'F'; });

    constexpr auto WSP = make_char_rule([](octet c) { return c == 0x20 || c == 0x09; });

    constexpr auto CR = make_char_rule([](octet c) { return c == 0x0D; });
    constexpr auto LF = make_char_rule([](octet c) { return c == 0x0A; });
    constexpr auto CRLF = CR + LF;

    // LWSP = *(WSP / (CRLF WSP))
    constexpr auto LWSP = zero_or_more(WSP || (CRLF + WSP));

    // 其他规则类似实现...

} // namespace mcs::common::ABNF

#include "../test_common/test_macro.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <string_view>

int main()
{
    using namespace mcs::common::ABNF; // NOLINT

    TEST("octet") = [] {
        static_assert(std::numeric_limits<octet>::min() == 0x00);
        static_assert(std::numeric_limits<octet>::max() == 0xFF); // NOLINT
    };
    TEST("ALPHA") = [] {
        static constexpr octet test_char = 'A';
        constexpr std::span<const octet> s(&test_char, 1);

        // 使用 static_assert 验证解析结果
        static_assert(ALPHA(s).success);
        static_assert(ALPHA(s).consumed == 1);

        static_assert(ALPHA(std::array<octet, 1>{'A'}).success); // 大写字母
        static_assert(ALPHA(std::array<octet, 1>{'M'}));
        // static_assert(ALPHA('Z'));
        // static_assert(ALPHA('a')); // 小写字母
        // static_assert(ALPHA('n'));
        // static_assert(ALPHA('z'));

        // static_assert(!ALPHA(' ')); // 非字母字符
        // static_assert(!ALPHA('@'));
        // static_assert(!ALPHA('['));
        // static_assert(!ALPHA('0'));
        // static_assert(!ALPHA('\t'));
    };

    TEST("BIT") = [] {
        // 有效BIT测试
        constexpr octet bit1[] = {'1'};
        static_assert(BIT(std::span<const octet>(bit1)).success);

        constexpr octet bit0[] = {'0'};
        static_assert(BIT(std::span<const octet>(bit0)).success);

        // 无效字符测试
        constexpr octet invalid[] = {'A'};
        static_assert(!BIT(std::span<const octet>(invalid)).success);

        // 空输入测试
        static_assert(!BIT(std::span<const octet>{}).success);
        // static_assert(BIT('1'));

        // static_assert(not BIT('A')); // 大写字母
    };
#if 0
    TEST("CHAR") = [] {
        static_assert(CHAR('0'));
        static_assert(CHAR('1'));

        static_assert(CHAR('A')); // 大写字母

        static_assert(CHAR(' '));

        constexpr char k_nul = '\0'; // 或 '\x00'
        static_assert(not CHAR(k_nul));
        static_assert(not CHAR('\x00'));

        // Character too large for enclosing character literal
        // typeclang(character_too_large)
        // static_assert(!CHAR('€')); // 非 ASCII

        // char c = '€'; // 编译期失败
        // NOTE: ASCII 是 0-127 即0x00 - 0x7F
        constexpr auto k_max = std::numeric_limits<char>::max();
        static_assert(k_max == 127); // NOLINT
        constexpr auto k_min = std::numeric_limits<char>::min();
        static_assert(k_min == -128); // NOLINT

        constexpr auto k_min2 = std::numeric_limits<unsigned char>::min();
        static_assert(k_min2 == 0);                                      // NOLINT
        static_assert(std::numeric_limits<unsigned char>::max() == 255); // NOLINT
        {
            constexpr char c = -42; // NOLINT
            // 合法，但值可能出乎意料！
            constexpr unsigned char uc = c; // NOLINT
            // unsigned char 的值为 256 + c（例如 -42 → 214）
            static_assert(uc == 214); // NOLINT
        }
    };

    TEST("CRLF") = [] {
        constexpr const auto *k_crlf = "\r\n"; // 序列
        constexpr std::string_view k_v = k_crlf;
        static_assert(k_v.length() == 2);
        static_assert(k_v.size() == 2);

        assert(k_v == "\r\n");
        {
            constexpr std::string_view k_v = "\r\n";
            static_assert(k_v.length() == 2);
            static_assert(k_v.size() == 2);
            assert(k_v == "\r\n");
        }
        {
            // NOTE: 运行时
            std::string k_s = "\r\n";
            std::string_view k_v = k_s;
            assert(k_v.length() == 2);
            assert(k_v.size() == 2);
            assert(k_v == "\r\n");
        }
    };
    TEST("CRLF 2") = [] {
        const auto *k_crlf = "\r\n";  // 序列
        const auto *k_crlf2 = "\r\n"; // 序列
        assert(k_crlf == k_crlf2);
        // static_assert(CRLF<k_crlf>); // is not a constant expressionc
        // static_assert(CRLF<"\r\n">); // 失败
    };

    TEST("CRLF 3 ") = [] {
        // NOLINTBEGIN
        constexpr octet valid[] = {CR, LF};
        static_assert(CRLF(valid));

        constexpr octet invalid1[] = {CR};
        static_assert(not CRLF(invalid1));

        constexpr octet invalid2[] = {LF, CR};
        static_assert(not CRLF(invalid2));

        constexpr octet invalid3[] = {0x00, LF};
        static_assert(not CRLF(invalid3));

        constexpr octet invalid4[] = {CR, LF, LF};
        static_assert(not CRLF(invalid4));
        // NOLINTEND
    };

    TEST("string_view") = [] {
        // std::string_view c = 'a'; // 不可以
        std::string_view c = "a"; // NOTE: string_view 是字符序列
        assert(c[0] == 'a');
    };

    TEST("CTL") = [] {
        // NOLINTBEGIN
        static_assert(CTL(0x00));
        static_assert(CTL(0x1F));
        static_assert(CTL(0x7F));

        static_assert(not CTL(0x20));
        static_assert(not CTL('A'));
        static_assert(not CTL(0x80));
        // NOLINTEND
    };

    TEST("DIGIT") = [] {
        static_assert(DIGIT('0'));
        static_assert(DIGIT('9'));

        static_assert(not DIGIT('/'));
        static_assert(not DIGIT(':'));
        static_assert(not DIGIT('A'));
    };

    TEST("Constants") = [] {
        static_assert(DQUOTE == '"');
        static_assert(HTAB == '\t');
        static_assert(LF == '\n');
        static_assert(CR == '\r');
        static_assert(SP == ' ');
    };

    TEST("HEXDIG") = [] {
        static_assert(HEXDIG('0'));
        static_assert(HEXDIG('9'));
        static_assert(HEXDIG('A'));
        static_assert(HEXDIG('F'));

        static_assert(not HEXDIG('G'));
        static_assert(not HEXDIG('a'));
        static_assert(not HEXDIG(0xFF)); // NOLINT
    };

    TEST("WSP") = [] {
        static_assert(WSP(SP));
        static_assert(WSP(HTAB));

        static_assert(not WSP('a'));
        static_assert(not WSP(0x00));
    };

    TEST("LWSP") = [] {
        // NOLINTBEGIN
        // 有效案例
        static_assert(LWSP(std::span<const octet>{})); // 空

        constexpr octet wsp1[] = {SP};
        static_assert(LWSP(wsp1));

        constexpr octet wsp2[] = {HTAB, SP, HTAB};
        static_assert(LWSP(wsp2));

        constexpr octet crlf_wsp[] = {CR, LF, SP};
        static_assert(LWSP(crlf_wsp));

        constexpr octet complex[] = {CR, LF, SP, HTAB, CR, LF, HTAB};
        static_assert(LWSP(complex));

        // 无效案例
        constexpr const octet crlf[] = {CR, LF};
        static_assert(not LWSP(crlf));

        constexpr octet bad_ending[] = {CR, LF, 'A'};
        static_assert(not LWSP(bad_ending));

        constexpr octet mixed[] = {SP, 'a'};
        static_assert(not LWSP(mixed));
        constexpr octet mixed2[] = {'a', SP};
        static_assert(not LWSP(mixed2));
        // NOLINTEND
    };

    TEST("VCHAR") = [] {
        // NOLINTBEGIN
        static_assert(VCHAR(0x21)); // '!'
        static_assert(VCHAR(0x7E)); // '~'
        static_assert(VCHAR('A'));

        static_assert(not VCHAR(0x20)); // SP
        static_assert(not VCHAR(0x7F));
        static_assert(not VCHAR(0x00));
        constexpr char k_nul = '\0'; // 或 '\x00'
        static_assert(not VCHAR(k_nul));
        // NOLINTEND
        static_assert(HTAB == '\t');
        static_assert(LF == '\n');
        static_assert(CR == '\r');
        static_assert(SP == ' ');
        static_assert(not VCHAR(HTAB));
        static_assert(not VCHAR(LF));
        static_assert(not VCHAR(CR));
        static_assert(not VCHAR(SP));
    };
#endif
    std::cout << "main done\n";
    return 0;
}