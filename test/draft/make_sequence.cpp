#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include "./__product_type.hpp"

using draft::__detail::product_type; // NOLINT

using octet = std::uint8_t;
using octets_view = std::span<const octet>;
using octets_view_in = const octets_view &;
using octet_in = const octet &;

using consumed_result = std::optional<std::size_t>;
constexpr auto make_consumed_result(size_t v) noexcept -> consumed_result // NOLINT
{
    return {v};
}

struct parser_ctx
{
    octets_view root_span;   // NOLINT
    std::size_t cur_index{}; // NOLINT
    std::size_t end_index{}; // NOLINT

    [[nodiscard]] constexpr auto size() const noexcept
    {
        return end_index - cur_index;
    }
    [[nodiscard]] constexpr auto empty() const noexcept
    {
        return end_index == cur_index;
    }
    [[nodiscard]] constexpr auto valid() const noexcept
    {
        return cur_index < end_index;
    }
};
using parser_ctx_ref = parser_ctx &;

struct transaction
{
  public:
    constexpr explicit transaction(parser_ctx_ref ctx) noexcept
        : ctx_{ctx}, index_{ctx.cur_index}
    {
    }
    constexpr void commit() noexcept
    {
        committed_ = true;
    }
    constexpr ~transaction() noexcept
    {
        if (not committed_)
            ctx_.cur_index = index_;
    }
    transaction(const transaction &) = delete;
    transaction(transaction &&) = delete;
    transaction &operator=(const transaction &) = delete;
    transaction &operator=(transaction &&) = delete;

  private:
    parser_ctx_ref ctx_;
    size_t index_;
    bool committed_{false};
};

template <typename Rule>
concept rule_concept = requires {
    typename Rule::rule_concept;
    typename Rule::result_type;
    typename Rule::result_type::domain;
    {
        Rule{}.parse(std::declval<parser_ctx_ref>())
    } noexcept -> std::same_as<std::optional<typename Rule::result_type>>;
    {
        Rule::build(std::declval<typename Rule::result_type>())
    } noexcept -> std::same_as<std::string>;
};

template <rule_concept Rule>
constexpr auto parse(parser_ctx_ref ctx) // NOLINT
{
    return Rule{}.parse(ctx);
}
////////////////////
struct rule_t;
constexpr octet to_lower(octet ch) noexcept // NOLINT
{
    constexpr std::uint8_t k_distance = 32;
    return (ch >= 'A' && ch <= 'Z') ? ch + k_distance : ch;
};
constexpr octet to_upper(octet ch) noexcept // NOLINT
{
    constexpr std::uint8_t k_distance = 32;
    return (ch >= 'a' && ch <= 'z') ? ch - k_distance : ch;
};
template <octet C>
struct CharInsensitive
{
    using rule_concept = rule_t;

    constexpr static auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
    {
        return !ctx.empty() && (to_upper(ctx.root_span[ctx.cur_index]) == C ||
                                to_lower(ctx.root_span[ctx.cur_index]) == C)
                   ? (ctx.cur_index += 1, make_consumed_result(1))
                   : std::nullopt;
    }
};
template <octet C>
struct CharSensitive
{
    using rule_concept = rule_t;

    constexpr static auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
    {
        return !ctx.empty() && ctx.root_span[ctx.cur_index] == C
                   ? (ctx.cur_index += 1, make_consumed_result(1))
                   : std::nullopt;
    }
};

template <typename Domain, typename Rule>
struct SimpleRule
{
  private:
    SimpleRule() = default;

  public:
    using rule_concept = rule_t;

    struct __type
    {
        using domain = Domain;
        octets_view value;
    };
    using result_type = __type;

    static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
    {
        auto ret = Rule{}(ctx);
        return ret ? make_consumed_result(*ret) : std::nullopt;
    }
    static constexpr auto parse(parser_ctx_ref ctx) noexcept -> std::optional<result_type>
    {
        auto begin{ctx.cur_index};
        auto ret = operator()(ctx);
        return ret ? std::make_optional(
                         result_type{.value = ctx.root_span.subspan(begin, *ret)})
                   : std::nullopt;
    }
    static constexpr auto build(const result_type &ctx) noexcept
    {
        return std::string(ctx.value.begin(), ctx.value.end());
    }
    friend Domain;
};
template <rule_concept... Rule>
struct make_alternative : product_type<Rule...>
{
    using rule_concept = rule_t;
    struct __type
    {
        using domain = make_alternative;
        using value_type = std::variant<std::monostate, typename Rule::result_type...>;
        __type &operator=(value_type &&v) noexcept
        {
            this->value = std::move(v);
            return *this;
        }
        value_type value; // NOLINT
    };
    using result_type = __type;

    constexpr auto parse(parser_ctx_ref ctx) noexcept -> std::optional<result_type>
    {
        transaction trans{ctx};
        using value_type = result_type::value_type;
        result_type ret;
        bool success = [&]<std::size_t... I>(std::index_sequence<I...>) noexcept {
            auto apply_one = [&]<std::size_t Idx>() noexcept {
                if (auto v = this->template get<Idx>().parse(ctx))
                {
                    ret.value = value_type{*v};
                    return true;
                }
                return false;
            };
            return static_cast<bool>((apply_one.template operator()<I>() || ...));
        }(std::make_index_sequence<sizeof...(Rule)>{});
        return success ? (trans.commit(), std::optional<result_type>{std::move(ret)})
                       : std::nullopt;
    }
    static constexpr auto build(const result_type &ctx) noexcept
    {
        return std::visit(
            [](auto &&arg) noexcept -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    return {};
                }
                else
                {
                    return T::domain::build(std::forward<decltype(arg)>(arg));
                }
            },
            ctx.value);
    }
};
template <typename... T>
make_alternative(T &&...r) -> make_alternative<std::decay_t<T>...>; // NOLINT

struct A_rule : SimpleRule<A_rule, CharSensitive<'A'>>
{
    using SimpleRule<A_rule, CharSensitive<'A'>>::SimpleRule;
};
struct B_rule : SimpleRule<B_rule, CharInsensitive<'B'>>
{
    using SimpleRule<B_rule, CharInsensitive<'B'>>::SimpleRule;
};
// A_rule::result_type;
static_assert(rule_concept<A_rule>);

template <rule_concept... Rule>
struct make_sequence : product_type<Rule...>
{
    using rule_concept = rule_t;
    struct __type
    {
        using domain = make_sequence;
        using value_type = product_type<typename Rule::result_type...>;
        value_type value;
    };
    using result_type = __type;
    constexpr auto parse(parser_ctx_ref ctx) noexcept -> std::optional<result_type>
    {
        transaction trans{ctx};
        result_type ret;
        bool success = [&]<std::size_t... I>(std::index_sequence<I...>) noexcept {
            auto apply_one = [&]<std::size_t Idx>() noexcept {
                if (auto v = this->template get<Idx>().parse(ctx))
                {
                    std::get<Idx>(ret.value) = std::move(*v);
                    return true;
                }
                return false;
            };
            return static_cast<bool>((apply_one.template operator()<I>() && ...));
        }(std::make_index_sequence<sizeof...(Rule)>{});
        return success ? (trans.commit(), std::optional<result_type>{std::move(ret)})
                       : std::nullopt;
    }
    static constexpr auto build(const result_type &ctx) noexcept
    {
        return ctx.value.apply([]<typename... T>(const T &...v) noexcept -> std::string {
            return ((T::domain::build(v) + ...) + "");
        });
    }
};
template <typename... T>
make_sequence(T &&...r) -> make_sequence<std::decay_t<T>...>; // NOLINT

template <size_t Min, size_t Max, rule_concept Rule>
struct make_repetition : product_type<Rule>
{
    using rule_concept = rule_t;
    struct __type
    {
        using domain = make_repetition;
        octets_view value;
    };
    using result_type = __type;

    constexpr auto parse(parser_ctx_ref ctx) noexcept -> std::optional<result_type>
    {
        transaction trans{ctx};
        auto &[rule] = *this;
        std::size_t begin = ctx.cur_index;
        std::size_t count = 0;
        while (count < Max && ctx.valid())
        {
            if (auto ret = rule.parse(ctx); not ret)
                break;
            ++count;
        }
        return (count >= Min)
                   ? (trans.commit(),
                      std::make_optional(result_type{
                          .value = ctx.root_span.subspan(begin, ctx.cur_index)}))
                   : std::nullopt;
    }
    static constexpr auto build(const result_type &ctx) noexcept
    {
        return std::string(ctx.value.begin(), ctx.value.end());
    }
};

int main()
{
    {
        octet str[] = {'A', 'B'}; // NOLINT
        std::span<octet> span{str};
        auto rule = make_alternative{A_rule{}, B_rule{}};
        parser_ctx ctx{.root_span = span, .cur_index = 0, .end_index = span.size()};

        auto ret = rule.parse(ctx);
        assert(ret);
        assert(ret.value().value.index() == 1);
        assert(ctx.cur_index == 1);
        assert(std::holds_alternative<A_rule::result_type>(ret.value().value));
        using Rule = decltype(rule);
        assert(Rule::build(ret.value()) == std::string{'A'});
        {
            auto ret = rule.parse(ctx);
            assert(ret);
            assert(ret.value().value.index() == 2);
            assert(ctx.cur_index == 2);
            assert(std::holds_alternative<B_rule::result_type>(ret.value().value));
            assert(Rule::build(ret.value()) == std::string{'B'});
        }
    }
    {
        octet str[] = {'A', 'B'}; // NOLINT
        std::span<octet> span{str};
        auto rule = make_sequence{A_rule{}, B_rule{}};
        using Rule = decltype(rule);

        parser_ctx ctx{.root_span = span, .cur_index = 0, .end_index = span.size()};
        auto ret = rule.parse(ctx);
        assert(ret);
        assert(ctx.cur_index == span.size());

        assert(Rule::build(ret.value()) == std::string{"AB"});
    }
    {
        {
            // 测试用例1：重复1-3次A_rule，输入"A"
            octet str1[] = {'A'}; // NOLINT
            std::span<octet> span1{str1};
            auto rule1 = make_repetition<1, 3, A_rule>{A_rule{}};
            using Rule1 = decltype(rule1);

            parser_ctx ctx1{
                .root_span = span1, .cur_index = 0, .end_index = span1.size()};
            auto ret1 = rule1.parse(ctx1);
            assert(ret1);
            assert(ctx1.cur_index == 1); // 匹配1次
            assert(Rule1::build(ret1.value()) == "A");

            // 测试用例2：重复1-3次A_rule，输入"AAA"
            octet str2[] = {'A', 'A', 'A'}; // NOLINT
            std::span<octet> span2{str2};
            parser_ctx ctx2{
                .root_span = span2, .cur_index = 0, .end_index = span2.size()};
            auto ret2 = rule1.parse(ctx2);
            assert(ret2);
            assert(ctx2.cur_index == 3); // 匹配3次
            assert(Rule1::build(ret2.value()) == "AAA");

            // 测试用例3：重复1-3次A_rule，输入"AAAB"（匹配3次后停止）
            octet str3[] = {'A', 'A', 'A', 'B'}; // NOLINT
            std::span<octet> span3{str3};        // NOLINT
            parser_ctx ctx3{
                .root_span = span3, .cur_index = 0, .end_index = span3.size()};
            auto ret3 = rule1.parse(ctx3);
            assert(ret3);
            assert(ctx3.cur_index == 3); // 匹配3次，不消耗'B'
            assert(Rule1::build(ret3.value()) == "AAA");

            // 测试用例4：重复1-3次A_rule，输入空（失败）
            octet str4[1] = {}; // NOLINT
            std::span<octet> span4{str4};
            parser_ctx ctx4{.root_span = span4, .cur_index = 0, .end_index = 0};
            auto ret4 = rule1.parse(ctx4);
            assert(!ret4); // 无法匹配（Min=1）
        }

        // 测试 Min=0 的情况（允许0次）
        {
            octet str[] = {'B'}; // 输入不匹配A_rule
            std::span<octet> span{str};
            auto rule = make_repetition<0, 3, A_rule>{A_rule{}};
            using Rule = decltype(rule);

            parser_ctx ctx{.root_span = span, .cur_index = 0, .end_index = span.size()};
            auto ret = rule.parse(ctx);
            assert(ret);                // 允许0次，匹配成功
            assert(ctx.cur_index == 0); // 未消耗任何字符
            assert(Rule::build(ret.value()) == "");
        }
    }
    {
        {
            // 测试用例1：重复匹配 "A" 或 "B" 的序列（例如 "ABAB"）
            using RepeatAB = make_repetition<2, 4, make_sequence<A_rule, B_rule>>;
            RepeatAB rule =
                RepeatAB{make_sequence{A_rule{}, B_rule{}}}; // 允许重复2-4次AB序列
            using Rule = decltype(rule);

            octet str1[] = {'A', 'B', 'A', 'B'}; // NOLINT (AB重复2次)
            std::span<octet> span1{str1};
            parser_ctx ctx1{
                .root_span = span1, .cur_index = 0, .end_index = span1.size()};
            auto ret1 = rule.parse(ctx1);
            assert(ret1);
            assert(ctx1.cur_index == 4); // 消耗4字符
            assert(Rule::build(ret1.value()) == "ABAB");

            octet str2[] = {'A', 'B', 'A',
                            'B', 'A', 'B'}; // NOLINT (AB重复3次，但Max=4允许)
            std::span<octet> span2{str2};
            parser_ctx ctx2{.root_span = span2, .cur_index = 0, .end_index = 6};
            auto ret2 = rule.parse(ctx2);
            assert(ret2);
            assert(ctx2.cur_index == 6);                   // 匹配3次
            assert(Rule::build(ret2.value()) == "ABABAB"); // 实际消耗6字符

            octet str3[] = {'A', 'B', 'C'}; // NOLINT (AB后接无效字符)
            std::span<octet> span3{str3};
            parser_ctx ctx3{.root_span = span3, .cur_index = 0, .end_index = 3};
            auto ret3 = rule.parse(ctx3);
            assert(not ret3);
            assert(ctx3.cur_index ==
                   0); // 匹配1次AB（Min=2次需要4字符，但输入不足，失败？需确认逻辑）
        }
        {
            // 测试用例2：混合选择与重复（例如 "AAAB" 或 "BBB"）
            using RepeatAOrB = make_alternative<make_repetition<3, 3, A_rule>, // 必须3次A
                                                make_repetition<2, 4, B_rule>  // 2-4次B
                                                >;
            RepeatAOrB rule = RepeatAOrB{make_repetition<3, 3, A_rule>{A_rule{}},
                                         make_repetition<2, 4, B_rule>{B_rule{}}};
            using Rule = decltype(rule);

            octet str1[] = {'A', 'A', 'A'}; // NOLINT (3次A)
            std::span<octet> span1{str1};
            parser_ctx ctx1{.root_span = span1, .cur_index = 0, .end_index = 3};
            auto ret1 = rule.parse(ctx1);
            assert(ret1);
            assert(ret1.value().value.index() == 1); // 匹配第一个选项（3次A）
            assert(Rule::build(ret1.value()) == "AAA");

            octet str2[] = {'B', 'B', 'B', 'B'}; // NOLINT (4次B)
            std::span<octet> span2{str2};
            parser_ctx ctx2{.root_span = span2, .cur_index = 0, .end_index = 4};
            auto ret2 = rule.parse(ctx2);
            assert(ret2);
            assert(ret2.value().value.index() == 2); // 匹配第二个选项（4次B）
            assert(Rule::build(ret2.value()) == "BBBB");

            octet str3[] = {'A', 'A', 'B'}; // NOLINT (无效，既不满足3次A，也不满足2次B）
            std::span<octet> span3{str3};
            parser_ctx ctx3{.root_span = span3, .cur_index = 0, .end_index = 3};
            auto ret3 = rule.parse(ctx3);
            assert(!ret3); // 无法匹配任何选项
        }
        {
            // 测试用例3：复杂嵌套（序列包含重复和选择）
            using ComplexRule = make_sequence<
                make_repetition<1, 2, A_rule>,                          // 1-2次A
                make_alternative<B_rule, make_sequence<A_rule, B_rule>> // B 或 AB
                >;
            ComplexRule rule = ComplexRule{
                make_repetition<1, 2, A_rule>{A_rule{}},
                make_alternative{B_rule{}, make_sequence{A_rule{}, B_rule{}}}};
            using Rule = decltype(rule);

            octet str1[] = {'A', 'B'}; // NOLINT (1次A + B)
            std::span<octet> span1{str1};
            parser_ctx ctx1{.root_span = span1, .cur_index = 0, .end_index = 2};
            auto ret1 = rule.parse(ctx1);
            assert(ret1);
            assert(Rule::build(ret1.value()) == "AB");

            octet str2[] = {'A', 'A', 'A', 'B'}; // NOLINT (2次A + AB)
            std::span<octet> span2{str2};
            parser_ctx ctx2{.root_span = span2, .cur_index = 0, .end_index = 4};
            auto ret2 = rule.parse(ctx2);
            assert(ret2);
            assert(ctx2.cur_index == 4); // 匹配2次A + AB
            assert(Rule::build(ret2.value()) == "AAAB");

            octet str3[] = {'A', 'C'}; // NOLINT (无效）
            std::span<octet> span3{str3};
            parser_ctx ctx3{.root_span = span3, .cur_index = 0, .end_index = 2};
            auto ret3 = rule.parse(ctx3);
            assert(!ret3);
        }
    }
    std::cout << "main done\n";
    return 0;
}