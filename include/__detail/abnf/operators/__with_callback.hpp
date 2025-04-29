#pragma once

#include "../detail/__concept.hpp"
#include <concepts>
#include <utility>

namespace mcs::abnf::operators
{
    template <detail::rule Rule, typename Callback>
        requires requires(Callback &callback, const detail::parser_ctx &ctx) {
            { callback(ctx) } noexcept -> std::same_as<void>;
        }
    struct with_callback
    {
        std::decay_t<Rule> rule; // NOLINT
        Callback &callback;      // NOLINT

        // NOLINTNEXTLINE
        constexpr with_callback(Rule &&r, Callback &c) noexcept
            : rule{std::forward<Rule>(r)}, callback{c}
        {
        }
        constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            auto ret = rule(ctx);
            if (ret)
                callback(ctx);
            return ret;
        }
    };
}; // namespace mcs::abnf::operators