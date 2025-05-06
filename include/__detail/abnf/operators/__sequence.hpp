#pragma once

#include "../detail/__concept.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"
#include "./__product_type.hpp"
#include <string>

namespace mcs::abnf::operators
{
    template <detail::rule... Rule>
    struct sequence : product_type<Rule...>
    {
        struct __type
        {
            using domain = sequence;
            using value_type = product_type<typename Rule::result_type...>;
            value_type value;
        };
        using result_type = __type;
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx_ref ctx) const noexcept
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
        constexpr auto parse(detail::parser_ctx_ref ctx) const noexcept
            -> std::optional<result_type>
        {
            transaction trans{ctx};
            result_type ret;
            bool success = [&]<std::size_t... I>(std::index_sequence<I...>) noexcept {
                auto apply_one = [&]<std::size_t Idx>() noexcept {
                    if (auto v = this->template get<Idx>().parse(ctx))
                    {
                        std::get<Idx>(ret.value) = std::move(*v);
                        return true;
                    }
                    return false;
                };
                return static_cast<bool>((apply_one.template operator()<I>() && ...));
            }(std::make_index_sequence<sizeof...(Rule)>{});
            return success ? (trans.commit(), std::optional<result_type>{std::move(ret)})
                           : std::nullopt;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            return ctx.value.apply(
                []<typename... T>(const T &...v) noexcept -> std::string {
                    return (T::domain::build(v) + ... + "");
                });
        }
    };
    template <typename... T>
    sequence(T &&...r) -> sequence<std::decay_t<T>...>;
    template <typename... T>
    inline constexpr bool is_operators_rule<sequence<T...>> = true; // NOLINT

}; // namespace mcs::abnf::operators