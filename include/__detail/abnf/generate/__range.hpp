#pragma once

#include "../detail/__types.hpp"
#include "../detail/__concept.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet begin, detail::octet end>
    struct Range
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::const_parser_ctx ctx) const noexcept
            -> detail::consumed_result
        {
            return !ctx.empty() && (begin <= ctx.root_span[ctx.cur_index] &&
                                    ctx.root_span[ctx.cur_index] <= end)
                       ? detail::make_consumed_result(1)
                       : std::nullopt;
        }
    };

}; // namespace mcs::abnf::generate