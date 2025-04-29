#pragma once

#include "../detail/__concept.hpp"
#include "./__product_type.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"

namespace mcs::abnf::operators
{
    template <detail::rule... Rule>
    struct make_sequence : product_type<Rule...>
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            std::size_t old_index = ctx.cur_index;
            auto apply_all = [&]<typename... R>(R &&...r) noexcept {
                return static_cast<bool>((std::forward<R>(r)(ctx) && ...))
                           ? (trans.commit(),
                              detail::make_consumed_result(ctx.cur_index - old_index))
                           : std::nullopt;
            };
            return this->apply(std::move(apply_all));
        }
    };

    template <typename... T>
    make_sequence(T &&...r) -> make_sequence<std::decay_t<T>...>;

    template <typename... T>
    inline constexpr bool is_operators_rule<make_sequence<T...>> = true; // NOLINT

}; // namespace mcs::abnf::operators