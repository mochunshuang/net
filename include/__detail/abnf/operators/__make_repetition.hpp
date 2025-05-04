#pragma once

#include "../detail/__concept.hpp"
#include "./__product_type.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"
#include <string>

namespace mcs::abnf::operators
{
    template <size_t Min, size_t Max, detail::enable_rule Rule>
    struct make_repetition : product_type<Rule>
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx_ref ctx) const noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            auto &rule = this->template get<0>();
            std::size_t old_index = ctx.cur_index;
            std::size_t count = 0;
            while (count < Max && ctx.valid())
            {
                if (auto ret = rule(ctx); not ret)
                    break;
                ++count;
            }
            return (count >= Min)
                       ? (trans.commit(),
                          detail::make_consumed_result(ctx.cur_index - old_index))
                       : std::nullopt;
        }

        struct __type
        {
            using domain = make_repetition;
            detail::octets_view value;
        };
        using result_type = __type;

        constexpr auto parse(detail::parser_ctx_ref ctx) const noexcept
            -> std::optional<result_type>
        {
            transaction trans{ctx};
            auto &rule = this->template get<0>();
            std::size_t begin = ctx.cur_index;
            std::size_t count = 0;
            while (count < Max && ctx.valid())
            {
                if (auto ret = rule.parse(ctx); not ret)
                    break;
                ++count;
            }
            return (count >= Min)
                       ? (trans.commit(),
                          std::make_optional(result_type{
                              .value = ctx.root_span.subspan(begin, ctx.cur_index)}))
                       : std::nullopt;
        }
        static constexpr auto build(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
    };

    template <size_t Min, size_t Max, typename T> // NOLINTNEXTLINE
    inline constexpr bool is_operators_rule<make_repetition<Min, Max, T>> = true;

}; // namespace mcs::abnf::operators