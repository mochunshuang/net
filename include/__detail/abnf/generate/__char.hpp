#pragma once

#include "../detail/__types.hpp"
#include "../tool/__to_upper.hpp"
#include "../tool/__to_lower.hpp"
#include "../detail/__concept.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet C>
    struct Char
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::const_parser_ctx ctx) const noexcept
            -> detail::consumed_result
        {
            return !ctx.empty() && (tool::to_upper(C) == ctx.root_span[ctx.cur_index] ||
                                    tool::to_lower(C) == ctx.root_span[ctx.cur_index])
                       ? detail::make_consumed_result(1)
                       : std::nullopt;
        }
    };
    template <detail::octet C>
    struct SensitiveChar
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::const_parser_ctx ctx) const noexcept
            -> detail::consumed_result
        {
            return !ctx.empty() && ctx.root_span[ctx.cur_index] == C
                       ? detail::make_consumed_result(1)
                       : std::nullopt;
        }
    };

}; // namespace mcs::abnf::generate