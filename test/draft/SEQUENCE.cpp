
#include <span>
#include <array>
#include <cstdint>
// NOLINTBEGIN
using OCTET = uint8_t;
using span_param_in = std::span<const OCTET>;

struct parse_result
{
    bool success;
    size_t consumed;
};

template <typename... Rules>
struct SequenceImpl;

template <>
struct SequenceImpl<>
{
    static constexpr parse_result parse(span_param_in)
    {
        return {true, 0};
    }
};

template <typename Rule, typename... Rules>
struct SequenceImpl<Rule, Rules...>
{
    static constexpr parse_result parse(span_param_in sp)
    {
        auto res = Rule{}(sp);
        if (!res.success)
            return {false, 0};

        auto remaining = sp.subspan(res.consumed);
        auto next = SequenceImpl<Rules...>::parse(remaining);
        return {next.success, res.consumed + next.consumed};
    }
};

template <typename... Rules>
constexpr auto SEQUENCE(Rules... rules)
{
    return [=](span_param_in sp) -> parse_result {
        return SequenceImpl<Rules...>::parse(sp);
    };
}

//=====================================================
// 使用示例
//=====================================================
constexpr auto rule_a = [](span_param_in sp) -> parse_result {
    return {!sp.empty() && sp[0] == 'A', 1};
};

constexpr auto rule_b = [](span_param_in sp) -> parse_result {
    return {.success = !sp.empty() && sp[0] == 'B', .consumed = 1};
};

constexpr auto rule_c = [](span_param_in sp) -> parse_result {
    return {.success = !sp.empty() && sp[0] == 'C', .consumed = 1};
};

// 组合三个规则：A -> B -> C
constexpr auto seq_abc = SEQUENCE(rule_a, rule_b, rule_c);

// 测试用例
static_assert(seq_abc(std::array<OCTET, 3>{'A', 'B', 'C'}).success);
static_assert(!seq_abc(std::array<OCTET, 3>{'A', 'X', 'C'}).success);

#include <iostream>

int main()
{
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND