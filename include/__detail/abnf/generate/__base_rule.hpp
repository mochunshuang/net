#pragma once

#include "../detail/__types.hpp"
#include "../operators/__operators_rule.hpp"

namespace mcs::abnf::generate
{
    template <operators::operators_rule Rule>
    struct base_rule
    {
        using rule_concept = detail::rule_t;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            if (auto ret = Rule{}(ctx))
                return make_consumed_result(*ret);
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::generate