#pragma once

#include "../detail/__types.hpp"
#include "./__fixedstring.hpp"
#include "../tool/__to_upper.hpp"
#include "../tool/__to_lower.hpp"
#include <optional>
#include <utility>

namespace mcs::abnf::generate
{
    template <detail::octet C>
    struct assert_not_is_char
    {
        using rule_concept = detail::rule_t;

        constexpr static auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            return ctx.done() || ctx.root_span[ctx.cur_index] != C
                       ? detail::make_consumed_result(0)
                       : std::nullopt;
        }
    };

    template <FixedString Str, typename Rule>
    struct assert_not_string
    {
        constexpr static size_t k_count = decltype(Str)::size - 1;

        constexpr static auto k_string = [] consteval {
            std::array<detail::octet, k_count> arr{};
            for (size_t i = 0; i < k_count; ++i)
            {
                arr[i] = static_cast<detail::octet>(Str.value[i]);
            }
            return arr;
        }();

        using rule_concept = detail::rule_t;
        using result_type =
            decltype(Rule{}.parse(std::declval<detail::parser_ctx_ref>()));

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            auto begin{ctx.cur_index};
            constexpr auto k_rule = Rule{};
            auto ret = k_rule(ctx);
            if (ret && ctx.cur_index - begin == k_count)
            {
                for (size_t i = 0; i < k_count; ++i)
                {
                    const auto k_current = ctx.root_span[begin + i];
                    if (tool::to_upper(k_current) == k_string[i] ||
                        tool::to_lower(k_current) == k_string[i])
                        return std::nullopt;
                }
            }
            return ret;
        }
        static constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
        {
            auto begin{ctx.cur_index};
            constexpr auto k_rule = Rule{};
            auto ret = k_rule.parse(ctx);
            if (ret && ctx.cur_index - begin == k_count)
            {
                for (size_t i = 0; i < k_count; ++i)
                {
                    const auto k_current = ctx.root_span[begin + i];
                    if (tool::to_upper(k_current) == k_string[i] ||
                        tool::to_lower(k_current) == k_string[i])
                        return std::nullopt;
                }
            }
            return ret;
        }
    };

    template <typename Rule>
    struct ctx_done_after
    {
        using rule_concept = detail::rule_t;
        using result_type =
            decltype(Rule{}.parse(std::declval<detail::parser_ctx_ref>()));

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            constexpr auto k_rule = Rule{};
            auto ret = k_rule(ctx);
            if (!ctx.done())
                return std::nullopt;
            return ret;
        }
        static constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
        {
            constexpr auto k_rule = Rule{};
            auto ret = k_rule.parse(ctx);
            if (!ctx.done())
                return std::nullopt;
            return ret;
        }
    };
}; // namespace mcs::abnf::generate