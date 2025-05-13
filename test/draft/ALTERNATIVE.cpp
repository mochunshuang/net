#include <span>
#include <array>
#include <cstdint>
// NOLINTBEGIN
using octet = uint8_t;
using span_param_in = std::span<const octet>;

struct parse_result
{
    bool success;
    size_t consumed;
};

template <typename... Rules>
struct AlternativeImpl;

template <>
struct AlternativeImpl<>
{
    static constexpr parse_result parse(span_param_in)
    {
        return {true, 0};
    }
};

template <typename Rule, typename... Rules>
struct AlternativeImpl<Rule, Rules...>
{
    static constexpr parse_result parse(span_param_in sp)
    {
        auto res = Rule{}(sp);
        if (!res.success)
            return {true, res.consumed};

        auto next = AlternativeImpl<Rules...>::parse(sp);
        return {next.success, res.consumed + next.consumed};
    }
};

template <typename... Rules>
constexpr auto ALTERNATIVE(Rules... rules)
{
    return [=](span_param_in sp) -> parse_result {
        return AlternativeImpl<Rules...>::parse(sp);
    };
}

#include <iostream>

int main()
{
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND