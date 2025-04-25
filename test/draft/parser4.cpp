#include <span>
#include <array>
#include <cstdint>

using OCTET = uint8_t;
using span_param_in = const std::span<const OCTET> &;

struct parse_result
{
    bool success;
    size_t consumed;
};

// NOLINTBEGIN

// 原子规则：CHAR
constexpr auto CHAR(OCTET c)
{
    return [=](span_param_in sp) -> parse_result {
        return {!sp.empty() && sp[0] == c, 1};
    };
}

// 顺序组合：SEQUENCE（参数包版本）
template <typename... Rules>
constexpr auto SEQUENCE(Rules... rules)
{
    return [=](span_param_in sp) -> parse_result {
        size_t total_consumed = 0;
        auto current_sp = sp;
        bool success = true;

        // 依次应用每个规则，使用立即调用lambda和折叠表达式
        (
            [&] {
                if (!success)
                    return;
                auto res = rules(current_sp);
                if (!res.success)
                {
                    success = false;
                    return;
                }
                total_consumed += res.consumed;
                current_sp = current_sp.subspan(res.consumed);
            }(),
            ...);

        return {success, total_consumed};
    };
}

// 选择组合：ALTERNATIVE（参数包版本）
template <typename... Rules>
constexpr auto ALTERNATIVE(Rules... rules)
{
    return [=](span_param_in sp) -> parse_result {
        parse_result final_result{false, 0};

        (
            [&] {
                if (final_result.success)
                    return;
                auto res = rules(sp);
                if (res.success)
                {
                    final_result = res;
                }
            }(),
            ...);

        return final_result;
    };
}

// 重复组合：REPETITION
template <size_t Min, size_t Max, typename Rule>
constexpr auto REPETITION(Rule rule)
{
    return [=](span_param_in sp) {
        size_t total = 0;
        size_t count = 0;
        while (count < Max && total < sp.size())
        {
            auto res = rule(sp.subspan(total));
            if (!res.success)
                break;
            total += res.consumed;
            ++count;
        }
        return parse_result{count >= Min, total};
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

// 测试代码
#include <iostream>
int main()
{
    {
        // 测试 CHAR 成功
        static_assert(CHAR('a')(std::array<OCTET, 1>{'a'}).success);
        static_assert(CHAR('a')(std::array<OCTET, 2>{'a', 'b'}).consumed == 1);

        // 测试 CHAR 失败
        static_assert(!CHAR('a')(std::array<OCTET, 1>{'b'}).success);
        static_assert(!CHAR('a')(std::array<OCTET, 0>{}).success); // 空输入
    }
    {
        // 成功序列
        constexpr auto seq_ab = SEQUENCE(CHAR('a'), CHAR('b'));
        static_assert(seq_ab(std::array<OCTET, 2>{'a', 'b'}).success);
        static_assert(seq_ab(std::array<OCTET, 3>{'a', 'b', 'c'}).consumed == 2);
        // 不是 BUG
        static_assert(seq_ab(std::array<OCTET, 3>{'a', 'b', 'c'}).success);

        // 失败情况（第一个字符不匹配）
        static_assert(!seq_ab(std::array<OCTET, 2>{'x', 'b'}).success);

        // 失败情况（第二个字符不匹配）
        static_assert(!seq_ab(std::array<OCTET, 2>{'a', 'x'}).success);
    }
    {
        // 成功选择第一个分支
        constexpr auto alt_ab = ALTERNATIVE(CHAR('a'), CHAR('b'));
        static_assert(alt_ab(std::array<OCTET, 1>{'a'}).success);
        static_assert(alt_ab(std::array<OCTET, 1>{'b'}).success);

        // 失败情况
        static_assert(!alt_ab(std::array<OCTET, 1>{'c'}).success);
    }
    {
        // 重复 2-3 次 'a'
        constexpr auto rep_2_3_a = REPETITION<2, 3>(CHAR('a'));

        // 成功：恰好 2 次
        static_assert(rep_2_3_a(std::array<OCTET, 2>{'a', 'a'}).success);
        static_assert(rep_2_3_a(std::array<OCTET, 2>{'a', 'a'}).consumed == 2);

        // 成功：3 次
        static_assert(rep_2_3_a(std::array<OCTET, 3>{'a', 'a', 'a'}).success);

        // 失败：只有 1 次
        static_assert(!rep_2_3_a(std::array<OCTET, 1>{'a'}).success);

        // 超过最大次数，只取前 3 次, 而且是成功的
        static_assert(rep_2_3_a(std::array<OCTET, 4>{'a', 'a', 'a', 'a'}).consumed == 3);
        static_assert(rep_2_3_a(std::array<OCTET, 4>{'a', 'a', 'a', 'a'}).success);
    }
    // 测试零次或多次（*）
    {
        constexpr auto rule = ZERO_OR_MORE(CHAR('a'));
        static_assert(rule(std::array<OCTET, 0>{}).success);         // 0次
        static_assert(rule(std::array<OCTET, 2>{'a', 'a'}).success); // 2次
        static_assert(rule(std::array<OCTET, 3>{'a', 'a', 'b'}).consumed ==
                      2); // 遇到非a停止
    }

    // 测试一次或多次（+）
    {
        constexpr auto rule = ONE_OR_MORE(CHAR('a'));
        static_assert(!rule(std::array<OCTET, 0>{}).success);   // 0次失败
        static_assert(rule(std::array<OCTET, 1>{'a'}).success); // 1次
        static_assert(rule(std::array<OCTET, 3>{'a', 'a', 'a'}).consumed == 3);
    }

    // 测试混合组合：a* [b] c+
    {
        constexpr auto complex_rule =
            SEQUENCE(ZERO_OR_MORE(CHAR('a')),
                     SEQUENCE(OPTIONAL(CHAR('b')), ONE_OR_MORE(CHAR('c'))));

        // 合法情况
        static_assert(complex_rule(std::array<OCTET, 1>{'c'}).success); // a0次 b无 c1次
        static_assert(
            complex_rule(std::array<OCTET, 3>{'a', 'b', 'c'}).success); // a1次 b有 c1次
        static_assert(
            complex_rule(std::array<OCTET, 3>{'a', 'c', 'c'}).success); // a1次 b无 c2次
        static_assert(complex_rule(std::array<OCTET, 2>{'b', 'c'}).success); // 没有 a

        // 非法情况
        static_assert(
            !complex_rule(std::array<OCTET, 3>{'a', 'b', 'x'}).success); // c无效
    }

    // 测试 REPETITION 边界
    {
        // 规则：2*3("a" / "b")
        constexpr auto rep_rule = REPETITION<2, 3>(ALTERNATIVE(CHAR('a'), CHAR('b')));

        // 合法情况
        static_assert(rep_rule(std::array<OCTET, 2>{'a', 'b'}).success);            // 2次
        static_assert(rep_rule(std::array<OCTET, 3>{'a', 'a', 'b'}).consumed == 3); // 3次

        // 边界情况
        static_assert(rep_rule(std::array<OCTET, 4>{'a', 'a', 'a', 'x'}).consumed ==
                      3); // 超过Max
        static_assert(rep_rule(std::array<OCTET, 2>{'a', 'x'}).success ==
                      false); // 中途失败
    }

    {
    }

    std::cout << "All tests passed!\n";
    return 0;
}
// NOLINTEND