#include <cstdint>
#include <optional>
#include <span>
#include <array>
#include <iostream>

using OCTET = uint8_t;
using span_param_in = const std::span<const OCTET> &;
using consumed_result = std::optional<size_t>;

//==================== 前向声明规则类型 ====================
struct CommentRule;
struct CContentRule;

//==================== 基础规则类型 ====================
template <OCTET C>
struct CharRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept;
};

template <OCTET Low, OCTET High>
struct RangeRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept;
};

//==================== 组合规则类型 ====================
template <typename... RuleTypes>
struct Sequence
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept;
};

template <typename... RuleTypes>
struct Alternative
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept;
};

//==================== 具体规则类定义 ====================
struct CommentRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept; // 声明
};

struct CContentRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept; // 声明
};

//==================== 成员函数实现 ====================
// CharRule实现
template <OCTET C>
constexpr consumed_result CharRule<C>::operator()(span_param_in sp) const noexcept
{
    return (!sp.empty() && sp[0] == C) ? std::make_optional(1) : std::nullopt;
}

// RangeRule实现
template <OCTET Low, OCTET High>
constexpr consumed_result RangeRule<Low, High>::operator()(
    span_param_in sp) const noexcept
{
    return (!sp.empty() && sp[0] >= Low && sp[0] <= High) ? std::make_optional(1)
                                                          : std::nullopt;
}

// Sequence实现
template <typename... RuleTypes>
constexpr consumed_result Sequence<RuleTypes...>::operator()(
    span_param_in sp) const noexcept
{
    size_t total = 0;
    auto remaining = sp;
    bool success = true;
    (([&] {
         if (!success)
             return;
         if (auto res = RuleTypes{}(remaining))
         {
             total += *res;
             remaining = remaining.subspan(*res);
         }
         else
         {
             success = false;
         }
     }()),
     ...);
    return success ? std::make_optional(total) : std::nullopt;
}

// Alternative实现
template <typename... RuleTypes>
constexpr consumed_result Alternative<RuleTypes...>::operator()(
    span_param_in sp) const noexcept
{
    consumed_result res;
    ((res = RuleTypes{}(sp)) || ...);
    return res;
}

// CContentRule实现（此时CommentRule已定义）
constexpr consumed_result CContentRule::operator()(span_param_in sp) const noexcept
{
    return Alternative<RangeRule<0x21, 0x27>,                           // ctext类型
                       Sequence<CharRule<'\\'>, RangeRule<0x00, 0xFF>>, // quoted-pair类型
                       CommentRule // 直接使用已定义类型
                       >{}(sp);
}

// CommentRule实现（此时CContentRule已定义）
constexpr consumed_result CommentRule::operator()(span_param_in sp) const noexcept
{
    return Sequence<CharRule<'('>,
                    decltype([](span_param_in sp) { // *( [FWS] ccontent )
                        constexpr RangeRule<0x09, 0x20> fws;
                        constexpr CContentRule cc;

                        size_t total = 0;
                        auto remaining = sp;
                        while (true)
                        {
                            size_t step = 0;
                            if (auto f = fws(remaining))
                            { // [FWS]
                                step += *f;
                                remaining = remaining.subspan(*f);
                            }
                            if (auto c = cc(remaining))
                            { // ccontent
                                step += *c;
                                remaining = remaining.subspan(*c);
                            }
                            else
                                break;
                            total += step;
                        }
                        return total > 0 ? std::make_optional(total) : std::nullopt;
                    }),
                    CharRule<')'>>{}(sp);
}

//==================== 测试验证 ====================
int main()
{
    // 编译期测试空注释: ()
    // static_assert(CommentRule{}(std::array<OCTET, 2>{'(', ')'}).value() == 2);
    // std::cout << CommentRule{}(std::array<OCTET, 2>{'(', ')'}).value() << '\n';

    // // 运行时测试嵌套注释: (a(b)c)
    // std::array<OCTET, 6> input{'(', 'a', '(', 'b', ')', ')'};
    // auto res = CommentRule{}(input);
    // // static_assert(res.value() == 6);
    // std::cout << "Consumed bytes: " << *res << "\n"; // 输出6

    return 0;
}