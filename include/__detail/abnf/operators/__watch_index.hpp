#pragma once

#include "../detail/__concept.hpp"
#include <concepts>
#include <utility>

namespace mcs::abnf::operators
{
    template <detail::rule Rule, typename Callback>
        requires requires(Callback &callback, const detail::parser_ctx &ctx) {
            {
                callback(auto(ctx.cur_index), auto(ctx.cur_index))
            } noexcept -> std::same_as<void>;
        }
    struct watch_index
    {
        using rule_concept = detail::rule_t;

        std::decay_t<Rule> rule; // NOLINT
        Callback &callback;      // NOLINT

        constexpr watch_index(Rule &&r, Callback &c) noexcept // NOLINT
            : rule{std::forward<Rule>(r)}, callback{c}
        {
        }
        constexpr auto operator()(detail::parser_ctx_ref ctx) const noexcept
            -> detail::consumed_result
        {
            auto index = ctx.cur_index;
            auto ret = rule(ctx);
            if (ret)
                callback(index, ctx.cur_index);
            return ret;
        }
    };
}; // namespace mcs::abnf::operators