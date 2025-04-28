#pragma once

#include "./__operable_rule.hpp"
#include "./__transaction.hpp"

namespace mcs::abnf::operators
{
    template <operable_rule... Rule>
    struct alternative
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx &ctx) const noexcept
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
}; // namespace mcs::abnf::operators