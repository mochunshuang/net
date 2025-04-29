#pragma once

#include "../detail/__concept.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"

namespace mcs::abnf::operators
{
    template <detail::rule... Rule>
    struct alternative
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx_ref ctx) const noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            detail::consumed_result res;
            ((res = Rule{}(ctx)) || ...);
            if (res)
                trans.commit();
            return res;
        }
    };
    template <typename... T>
    inline constexpr bool is_operators_rule<alternative<T...>> = true; // NOLINT

}; // namespace mcs::abnf::operators