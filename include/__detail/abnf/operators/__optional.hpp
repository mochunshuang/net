#pragma once

#include "../detail/__concept.hpp"
#include "./__product_type.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"
#include <variant>
#include <string>

namespace mcs::abnf::operators
{
    template <detail::enable_rule Rule>
    struct optional : product_type<Rule>
    {
        using rule_concept = detail::rule_t;
        using result_type = std::variant<std::monostate, typename Rule::result_type>;

        constexpr auto operator()(detail::parser_ctx_ref ctx) const noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            std::size_t old_index = ctx.cur_index;
            auto ret = this->template get<0>()(ctx);
            return ret ? (trans.commit(),
                          detail::make_consumed_result(ctx.cur_index - old_index))
                       : detail::make_consumed_result(0);
        }
        constexpr auto parse(detail::parser_ctx_ref ctx) const noexcept
            -> std::optional<result_type>
        {
            transaction trans{ctx};
            if constexpr (std::same_as<std::optional<typename Rule::result_type>,
                                       decltype(this->template get<0>().parse(ctx))>)
            {
                auto ret = this->template get<0>().parse(ctx);
                return ret ? (trans.commit(), result_type{std::move(*ret)})
                           : result_type{};
            }
            else
            {
                auto ret = this->template get<0>().parse(ctx);
                return ret ? (trans.commit(), result_type{std::move(ret)})
                           : result_type{};
            }
        }
        static constexpr std::string buildString(const result_type &ctx) noexcept
        {
            return ctx.index == 0 ? std::string{} : result_type ::domain::build(ctx);
        }
    };

    template <typename T>
    optional(T &&r) -> optional<std::decay_t<T>>;

    template <typename T> // NOLINTNEXTLINE
    inline constexpr bool is_operators_rule<optional<T>> = true;

}; // namespace mcs::abnf::operators