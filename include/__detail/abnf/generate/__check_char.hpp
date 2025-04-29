#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::generate
{
    template <detail::octet C>
    struct assert_not_is_char
    {
        using rule_concept = detail::rule_t;

        constexpr static auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            return ctx.empty() || ctx.root_span[ctx.cur_index] != C
                       ? detail::make_consumed_result(0)
                       : std::nullopt;
        }
    };
}; // namespace mcs::abnf::generate