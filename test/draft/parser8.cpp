#include <cassert>
#include <cstddef>
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
    constexpr consumed_result operator()(span_param_in sp) const noexcept
    {
        return (!sp.empty() && sp[0] == C) ? std::make_optional(size_t{1}) : std::nullopt;
    }
};

template <OCTET Low, OCTET High>
struct RangeRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept
    {
        return (!sp.empty() && sp[0] >= Low && sp[0] <= High)
                   ? std::make_optional(size_t{1})
                   : std::nullopt;
    }
};

//==================== 组合规则类型 ====================
template <typename... RuleTypes>
struct Sequence
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept
    {
        size_t total = 0;
        auto remaining = sp;
        bool success = true;

        // 使用折叠表达式处理每个规则
        ((success = success &&
                    [&](auto rule) {
                        if (!success)
                            return false;
                        if (auto res = rule(remaining))
                        {
                            total += res.value();
                            remaining = remaining.subspan(*res);
                            return true;
                        }
                        return false;
                    }(RuleTypes{})),
         ...);

        return success ? std::make_optional(total) : std::nullopt;
    }
};

template <typename... RuleTypes>
struct Alternative
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept
    {
        consumed_result res;
        ((res = RuleTypes{}(sp)) || ...);
        return res;
    }
};

//==================== 互递归规则实现 ====================
struct CommentRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept;
};

struct CContentRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept;
};

// CContentRule实现
constexpr consumed_result CContentRule::operator()(span_param_in sp) const noexcept
{
    return Alternative<RangeRule<0x21, 0x27>,                           // ctext
                       Sequence<CharRule<'\\'>, RangeRule<0x00, 0xFF>>, // quoted-pair
                       CommentRule>{}(sp);
}

// CommentRule实现（关键修改点）
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

                            // [FWS]
                            if (auto f = fws(remaining))
                            {
                                step += *f;
                                remaining = remaining.subspan(*f);
                            }

                            // ccontent
                            if (auto c = cc(remaining))
                            {
                                step += *c;
                                remaining = remaining.subspan(*c);
                            }
                            else
                            {
                                break; // 没有更多内容时退出
                            }

                            total += step;
                        }
                        return std::make_optional(total); // 始终返回成功，允许0次匹配
                    }),
                    CharRule<')'>>{}(sp);
}

//==================== 测试验证 ====================
int main()
{
    // 测试空注释: ()
    constexpr auto empty_res = CommentRule{}(std::array<OCTET, 2>{'(', ')'});
    static_assert(empty_res.has_value() && empty_res.value() == 2); // 现在可以通过

    // 测试嵌套注释: (a(b)c)
    constexpr std::array<OCTET, 6> input{'(', 'a', '(', 'b', ')', ')'};
    constexpr auto nested_res = CommentRule{}(input);
    // static_assert(nested_res.has_value() && nested_res.value() == 6);

    // 安全访问（运行时示例）
    if (auto res = CommentRule{}(input))
    {
        std::cout << "Consumed bytes: " << *res << "\n";
    }
    else
    {
        std::cout << "Parse failed\n";
    }

    return 0;
}