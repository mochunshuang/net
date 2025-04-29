#pragma once

#include "../detail/__concept.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"

namespace mcs::abnf::operators
{
    template <detail::rule... Rule>
    struct sequence
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx_ref ctx) const noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            std::size_t old_index = ctx.cur_index;
            return static_cast<bool>((Rule{}(ctx) && ...))
                       ? (trans.commit(),
                          detail::make_consumed_result(ctx.cur_index - old_index))
                       : std::nullopt;
        }
    };

    template <typename... T>
    inline constexpr bool is_operators_rule<sequence<T...>> = true; // NOLINT

}; // namespace mcs::abnf::operators