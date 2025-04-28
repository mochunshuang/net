#pragma once

#include "./__operable_rule.hpp"
#include "./__product_type.hpp"
#include "./__transaction.hpp"

namespace mcs::abnf::operators
{
    template <operable_rule... Rule>
    struct make_sequence : product_type<Rule...>
    {
        constexpr auto operator()(detail::parser_ctx &ctx) noexcept
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

}; // namespace mcs::abnf::operators