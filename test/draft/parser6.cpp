#include <cassert>
#include <iostream>
#include <optional>
#include <span>
#include <array>
#include <cstdint>

using octet = uint8_t;
using span_param_in = const std::span<const octet> &;

using consumed_result = std::optional<std::size_t>;

// 原子规则：CHAR
constexpr auto CHAR(octet c)
{
    return [=](span_param_in sp) -> consumed_result {
        return (!sp.empty() && sp[0] == c) ? std::make_optional(1) : std::nullopt;
    };
}

// 顺序组合：SEQUENCE（参数包版本）
template <typename... Rules>
constexpr auto SEQUENCE(Rules... rules)
{
    return [=](span_param_in sp) constexpr noexcept -> consumed_result {
        std::size_t total = 0;
        auto remaining = sp;
        bool success = true;

        (
            [&](auto rule) constexpr noexcept {
                if (!success)
                    return;
                if (auto res = rule(remaining))
                {
                    total += *res;
                    remaining = remaining.subspan(*res);
                }
                else
                {
                    success = false;
                }
            }(rules),
            ...);

        return success ? std::make_optional(total) : std::nullopt;
    };
}

// 选择组合：ALTERNATIVE（参数包版本）
template <typename... Rules>
constexpr auto ALTERNATIVE(Rules... rules)
{
    return [=](span_param_in sp) constexpr noexcept -> consumed_result {
        consumed_result result;
        (
            [&](auto rule) constexpr noexcept {
                if (result)
                    return;
                if (auto res = rule(sp))
                {
                    result = res;
                }
            }(rules),
            ...);
        return result;
    };
}

// 重复组合：REPETITION
template <size_t Min, size_t Max, typename Rule>
constexpr auto REPETITION(Rule rule)
{
    return [=](span_param_in sp) -> consumed_result {
        std::size_t total = 0;
        std::size_t count = 0;

        while (count < Max && total <= sp.size())
        {
            auto res = rule(sp.subspan(total));
            if (!res)
                break;
            total += *res;
            ++count;
        }
        return (count >= Min) ? std::make_optional(total) : std::nullopt;
    };
}

// 语法糖组合子
template <typename Rule>
constexpr auto ZERO_OR_MORE(Rule rule)
{
    return REPETITION<0, SIZE_MAX>(rule);
}

template <typename Rule>
constexpr auto ONE_OR_MORE(Rule rule)
{
    return REPETITION<1, SIZE_MAX>(rule);
}

template <typename Rule>
constexpr auto OPTIONAL(Rule rule)
{
    return REPETITION<0, 1>(rule);
}

// 字符范围匹配（闭区间）
constexpr auto RANGE(octet low, octet high)
{
    return [=](span_param_in sp) -> consumed_result {
        if (!sp.empty() && sp[0] >= low && sp[0] <= high)
            return 1;
        return std::nullopt;
    };
}

// 基础原子规则
constexpr auto CR = CHAR(0x0D);     // %x0D
constexpr auto LF = CHAR(0x0A);     // %x0A
constexpr auto SP = CHAR(0x20);     // %x20
constexpr auto HTAB = CHAR(0x09);   // %x09
constexpr auto DQUOTE = CHAR(0x22); // %x22

// 组合规则
constexpr auto CRLF = SEQUENCE(CR, LF); // CR LF

constexpr auto CTL = ALTERNATIVE( // %x00-1F / %x7F
    RANGE(0x00, 0x1F), CHAR(0x7F));

constexpr auto DIGIT = RANGE(0x30, 0x39); // 0-9
constexpr auto ALPHA = ALTERNATIVE(       // A-Z / a-z
    RANGE(0x41, 0x5A), RANGE(0x61, 0x7A));
constexpr auto HEXDIG = ALTERNATIVE( // DIGIT / "A" / "B" / ... / "F"
    DIGIT, ALTERNATIVE(CHAR('A'), CHAR('B'), CHAR('C'), CHAR('D'), CHAR('E'), CHAR('F')),
    ALTERNATIVE(CHAR('a'), CHAR('b'), CHAR('c'), CHAR('d'), CHAR('e'), CHAR('f')));

constexpr auto WSP = ALTERNATIVE(SP, HTAB); // SP / HTAB

// 复杂规则
constexpr auto LWSP = ZERO_OR_MORE( // *(WSP / CRLF WSP)
    ALTERNATIVE(WSP, SEQUENCE(CRLF, WSP)));

constexpr auto BIT = ALTERNATIVE(CHAR('0'), CHAR('1')); // "0" / "1"

constexpr auto VCHAR = RANGE(0x21, 0x7E);     // %x21-7E
constexpr auto CHAR_rule = RANGE(0x01, 0x7F); // %x01-7F (exclude NUL)

// octet 直接匹配任意字节（总是成功消耗1字节）
constexpr auto OCTET_rule = [](span_param_in sp) -> consumed_result {
    return sp.empty() ? std::nullopt : std::make_optional(1);
};

constexpr auto CHAR_CI(octet c)
{
    constexpr auto to_lower = [](octet ch) constexpr {
        return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch;
    };
    constexpr auto to_upper = [](octet ch) constexpr {
        return (ch >= 'a' && ch <= 'z') ? ch - 32 : ch;
    };
    return ALTERNATIVE(CHAR(to_lower(c)), CHAR(to_upper(c)));
}

/// 固定次数重复（语法糖）
template <size_t N, typename Rule>
constexpr auto TIMES(Rule rule)
{
    static_assert(N != 0, "Use REPETITION directly for 0+ repetitions");
    return REPETITION<N, N>(rule);
}

/// 否定匹配（消耗1字节当且仅当规则不匹配）
template <typename Rule>
constexpr auto NOT(Rule rule)
{
    return [=](span_param_in sp) -> consumed_result {
        return (sp.empty() || rule(sp)) ? std::nullopt : std::make_optional(1);
    };
}

/// 正向预查（匹配规则但不消耗输入）
template <typename Rule>
constexpr auto LOOKAHEAD(Rule rule)
{
    return [=](span_param_in sp) -> consumed_result {
        return rule(sp).has_value() ? std::make_optional(0) : std::nullopt;
    };
}

/// 分隔符列表 (element (sep element)*)
template <typename ElementRule, typename SepRule>
constexpr auto SEPARATED_BY(ElementRule elem, SepRule sep)
{
    return SEQUENCE(elem, ZERO_OR_MORE(SEQUENCE(sep, elem)));
}

/// 字符集匹配（语法糖）
template <typename... OCTETs>
constexpr auto ANY_OF(OCTETs... cs)
{
    return ALTERNATIVE(CHAR(cs)...);
}

// 测试代码
int main()
{
    {
        // 测试 CHAR
        static_assert(CHAR('a')(std::array<octet, 1>{'a'}).value() == 1);
        static_assert(!CHAR('a')(std::array<octet, 1>{'b'}));

        // 测试 SEQUENCE
        constexpr auto seq = SEQUENCE(CHAR('a'), CHAR('b'));
        static_assert(seq(std::array<octet, 2>{'a', 'b'}).value() == 2);
        static_assert(!seq(std::array<octet, 2>{'a', 'c'}));

        // 测试 ALTERNATIVE
        constexpr auto alt = ALTERNATIVE(CHAR('a'), CHAR('b'));
        static_assert(alt(std::array<octet, 1>{'a'}).value() == 1);
        static_assert(alt(std::array<octet, 1>{'b'}).value() == 1);

        // 测试 REPETITION
        constexpr auto rep = REPETITION<2, 3>(CHAR('a'));
        static_assert(rep(std::array<octet, 2>{'a', 'a'}).value() == 2);
        static_assert(!rep(std::array<octet, 1>{'a'}));
    }

    // 测试1：多字节规则
    {
        // 匹配连续两个相同字符（消耗2字节）
        constexpr auto DOUBLE_CHAR = [](octet c) {
            return SEQUENCE(CHAR(c), CHAR(c));
        };

        // 成功匹配
        static_assert(DOUBLE_CHAR('x')(std::array<octet, 2>{'x', 'x'}).value() == 2);
        // 失败情况（第二个字符不匹配）
        static_assert(!DOUBLE_CHAR('x')(std::array<octet, 2>{'x', 'y'}));
        // 失败情况（输入不足）
        static_assert(!DOUBLE_CHAR('x')(std::array<octet, 1>{'x'}));
    }

    // 测试2：嵌套组合 + 多字节规则
    {
        // 规则：(A A / B B)(C+)
        constexpr auto complex_rule =
            SEQUENCE(ALTERNATIVE(SEQUENCE(CHAR('A'), CHAR('A')), // 消耗2字节
                                 SEQUENCE(CHAR('B'), CHAR('B'))  // 消耗2字节
                                 ),
                     ONE_OR_MORE(CHAR('C')) // 至少消耗1字节
            );

        // 成功案例1：AACC（总消耗4）
        static_assert(complex_rule(std::array<octet, 4>{'A', 'A', 'C', 'C'}).value() ==
                      4);
        // 成功案例2：BBCCC（总消耗5）
        static_assert(
            complex_rule(std::array<octet, 5>{'B', 'B', 'C', 'C', 'C'}).value() == 5);
        // 失败案例：BBC（ONE_OR_MORE需要至少1个C，这里成功但总长度需要验证）
        static_assert(complex_rule(std::array<octet, 3>{'B', 'B', 'C'}).value() ==
                      3); // 应成功
        // 失败案例：AX（第一个ALTERNATIVE分支失败）
        static_assert(!complex_rule(std::array<octet, 2>{'A', 'X'}));
    }

    // 测试3：动态消耗规则
    {
        // 规则：匹配数字后跟对应数量的字符（例如 '3abc' 消耗4字节）
        constexpr auto DYNAMIC_RULE = [](span_param_in sp) -> consumed_result {
            if (sp.empty() || sp[0] < '1' || sp[0] > '9')
                return std::nullopt;

            const size_t count = sp[0] - '0';
            if (sp.size() < count + 1)
                return std::nullopt;

            return count + 1; // 返回数字本身+后续字符的总消耗
        };

        // 组合测试：数字规则后跟固定后缀
        constexpr auto suffix_rule = SEQUENCE(DYNAMIC_RULE, CHAR('!'));

        // 成功案例：'3abc!'（消耗5）
        static_assert(
            suffix_rule(std::array<octet, 5>{'3', 'a', 'b', 'c', '!'}).value() == 5);
        // 失败案例1：数字超出范围
        static_assert(!suffix_rule(std::array<octet, 3>{'0', 'a', '!'}));
        // 失败案例2：后续字符不足
        static_assert(!suffix_rule(std::array<octet, 4>{'3', 'a', 'b', '!'}));
    }

    // 测试4：多层重复嵌套
    {
        // 规则：((a b)+ | (c d)*) (e{2,3})
        constexpr auto monster_rule = SEQUENCE(
            ALTERNATIVE(ONE_OR_MORE(SEQUENCE(CHAR('a'), CHAR('b'))), // 每次消耗2字节
                        ZERO_OR_MORE(SEQUENCE(CHAR('c'), CHAR('d'))) // 每次消耗2字节
                        ),
            REPETITION<2, 3>(CHAR('e')) // 消耗2或3字节
        );

        // 成功案例1：ababeeee（消耗8）
        static_assert(
            monster_rule(std::array<octet, 8>{'a', 'b', 'a', 'b', 'e', 'e', 'e', 'e'})
                .value() == 7); // 前4字节(abab) + 后3字节(eee) -> 只取前3个e

        // 成功案例2：cdee（消耗4）
        static_assert(monster_rule(std::array<octet, 4>{'c', 'd', 'e', 'e'}).value() ==
                      4);

        // 失败案例1：只有1个e
        static_assert(!monster_rule(std::array<octet, 3>{'a', 'b', 'e'}));
    }

    // 测试5：边界值压力测试
    {
        // 规则：a{0,}（等同于ZERO_OR_MORE）
        constexpr auto infinite_a = ZERO_OR_MORE(CHAR('a'));

        // 空输入成功（消耗0）
        static_assert(infinite_a(std::array<octet, 0>{}).value() == 0);
        // 极限长度测试（1MB数据，此处用大数组模拟）
        std::array<octet, 1024 * 1024> giant_array = [] constexpr {
            std::array<octet, 1024 * 1024> arr{};
            arr.fill('a');
            return arr;
        }();
        assert(infinite_a(giant_array).value() == 1024 * 1024);
    }

    {
        // 测试 CTL
        static_assert(CTL(std::array<octet, 1>{0x00}).has_value());
        static_assert(CTL(std::array<octet, 1>{0x1F}).has_value());
        static_assert(CTL(std::array<octet, 1>{0x7F}).has_value());
        static_assert(!CTL(std::array<octet, 1>{0x80}));

        // 测试 CRLF
        static_assert(CRLF(std::array<octet, 2>{0x0D, 0x0A}).value() == 2);
        static_assert(!CRLF(std::array<octet, 2>{0x0D, 0x0B}));

        // 测试 LWSP
        {
            // 合法案例: 多个 WSP 和 CRLF WSP 组合
            constexpr auto sp1 = std::array<octet, 5>{0x20, 0x09, 0x0D, 0x0A, 0x20};
            static_assert(LWSP(sp1).value() == 5);

            // 空输入
            static_assert(LWSP(std::array<octet, 0>{}).value() == 0);
        }

        // 测试 HEXDIG
        static_assert(HEXDIG(std::array<octet, 1>{'A'}).has_value());
        static_assert(HEXDIG(std::array<octet, 1>{'f'}).has_value());
        static_assert(!HEXDIG(std::array<octet, 1>{'G'}));

        // 测试 OCTET_rule
        static_assert(OCTET_rule(std::array<octet, 1>{0xFF}).value() == 1);
        static_assert(!OCTET_rule(std::array<octet, 0>{}));

        // 测试 BIT
        static_assert(BIT(std::array<octet, 1>{'0'}).has_value());
        static_assert(BIT(std::array<octet, 1>{'1'}).has_value());
        static_assert(!BIT(std::array<octet, 1>{'2'}));
    }
    {
        // 实现 HTTP 版本规则 "HTTP/" DIGIT "." DIGIT
        constexpr auto http_version =
            SEQUENCE(SEQUENCE(CHAR('H'), CHAR('T'), CHAR('T'), CHAR('P'), CHAR('/')),
                     DIGIT, CHAR('.'), DIGIT);
        static_assert(
            http_version(std::array<octet, 8>{'H', 'T', 'T', 'P', '/', '1', '.', '1'})
                .has_value());
    }
    {
        // 测试 CHAR_CI
        static_assert(CHAR_CI('K')(std::array<octet, 1>{'k'}).has_value());
        static_assert(CHAR_CI('5')(std::array<octet, 1>{'5'}).has_value());
    }
    {
        //--------------- RFC 5234 增强实现 ---------------
        // 使用新组合子简化既有规则
        constexpr auto HEXDIG =
            ALTERNATIVE(DIGIT, SEPARATED_BY(CHAR_CI('A'), CHAR('-')) // 演示复杂组合
            );
        static_assert(HEXDIG(std::array<octet, 1>{'A'}).has_value());
        static_assert(HEXDIG(std::array<octet, 3>{'a', '-', 'A'}).has_value());
        static_assert(!HEXDIG(std::array<octet, 1>{'G'}));

        constexpr auto LWSP = ZERO_OR_MORE(
            ALTERNATIVE(WSP, SEQUENCE(CRLF, LOOKAHEAD(WSP)) // 确保CRLF后必须有WSP
                        ));
        // 合法案例: 多个 WSP 和 CRLF WSP 组合
        constexpr auto sp1 = std::array<octet, 5>{0x20, 0x09, 0x0D, 0x0A, 0x20};
        static_assert(LWSP(sp1).value() == 5);

        // 空输入
        static_assert(LWSP(std::array<octet, 0>{}).value() == 0);
    }
    {
        // 测试 LOOKAHEAD
        constexpr auto quoted = SEQUENCE(DQUOTE,
                                         LOOKAHEAD(NOT(DQUOTE)), // 确保引号内非空
                                         ONE_OR_MORE(DIGIT), DQUOTE);
        static_assert(quoted(std::array<octet, 5>{'"', '1', '2', '3', '"'}).has_value());
    }

    std::cout << "All tests passed!\n";
    return 0;
}