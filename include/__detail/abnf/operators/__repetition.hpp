#pragma once

#include "./__operable_rule.hpp"

namespace mcs::abnf::operators
{
    template <size_t Min, size_t Max, operable_rule Rule>
    struct repetition
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::const_parser_ctx ctx) const noexcept
            -> detail::consumed_result
        {
            std::size_t total = 0;
            std::size_t count = 0;
            detail::parser_ctx copy_ctx = ctx;
            while (count <= Max && copy_ctx.valid())
            {
                auto res = Rule{}(copy_ctx);
                if (!res)
                    break;
                total += *res;
                copy_ctx.cur_index += *res;
                ++count;
            }
            return (count >= Min) ? detail::make_consumed_result(total) : std::nullopt;
        };
    };
}; // namespace mcs::abnf::operators