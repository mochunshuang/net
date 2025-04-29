#pragma once

#include "../detail/__types.hpp"
#include "../tool/__to_upper.hpp"
#include "../tool/__to_lower.hpp"
#include "../detail/__concept.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet C>
    struct CharInsensitive
    {
        using rule_concept = detail::rule_t;

        constexpr static auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            return !ctx.empty() && (tool::to_upper(ctx.root_span[ctx.cur_index]) == C ||
                                    tool::to_lower(ctx.root_span[ctx.cur_index]) == C)
                       ? (ctx.cur_index += 1, detail::make_consumed_result(1))
                       : std::nullopt;
        }
    };
    template <detail::octet C>
    struct CharSensitive
    {
        using rule_concept = detail::rule_t;

        constexpr static auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            return !ctx.empty() && ctx.root_span[ctx.cur_index] == C
                       ? (ctx.cur_index += 1, detail::make_consumed_result(1))
                       : std::nullopt;
        }
    };

}; // namespace mcs::abnf::generate