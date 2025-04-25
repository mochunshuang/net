#pragma once

#include "./__operable_rule.hpp"

namespace mcs::abnf::operators
{
    template <operable_rule... Rule>
    struct alternative
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::const_parser_ctx ctx) const noexcept
            -> detail::consumed_result
        {
            detail::consumed_result res;
            ((res = Rule{}(ctx)) || ...);
            return res;
        }
    };
}; // namespace mcs::abnf::operators