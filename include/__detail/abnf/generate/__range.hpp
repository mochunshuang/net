#pragma once

#include "../detail/__types.hpp"
#include "../detail/__concept.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet begin, detail::octet end>
    struct Range
    {
        using rule_concept = detail::rule_t;

        constexpr static auto operator()(detail::parser_ctx &ctx) noexcept
            -> detail::consumed_result
        {
            return !ctx.empty() && (begin <= ctx.root_span[ctx.cur_index] &&
                                    ctx.root_span[ctx.cur_index] <= end)
                       ? (ctx.cur_index += 1, detail::make_consumed_result(1))
                       : std::nullopt;
        }
    };

}; // namespace mcs::abnf::generate