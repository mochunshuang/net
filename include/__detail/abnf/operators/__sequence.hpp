#pragma once

#include "./__operable_rule.hpp"
#include "./__transaction.hpp"
#include <utility>

namespace mcs::abnf::operators
{
    template <operable_rule... Rule>
    struct sequence
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx &ctx) const noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            std::size_t old_index = ctx.cur_index;
            auto apply_rule = [&]<typename R>(R &&r) noexcept -> bool {
                return static_cast<bool>(std::forward<R>(r)(ctx));
            };
            return static_cast<bool>((apply_rule(Rule{}) && ...))
                       ? (trans.commit(),
                          detail::make_consumed_result(ctx.cur_index - old_index))
                       : std::nullopt;
        }
    };
}; // namespace mcs::abnf::operators