#pragma once

#include "../detail/__concept.hpp"
#include "./__product_type.hpp"
#include "./__transaction.hpp"
#include "./__operators_rule.hpp"
#include <string>
#include <variant>

namespace mcs::abnf::operators
{
    template <detail::enable_rule... Rule>
    struct make_alternative : product_type<Rule...>
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            std::size_t old_index = ctx.cur_index;
            auto apply_all = [&]<typename... R>(R &&...r) noexcept {
                return static_cast<bool>((std::forward<R>(r)(ctx) || ...))
                           ? (trans.commit(),
                              detail::make_consumed_result(ctx.cur_index - old_index))
                           : std::nullopt;
            };
            return this->apply(std::move(apply_all));
        }
        struct __type
        {
            using domain = make_alternative;
            using value_type =
                std::variant<std::monostate, typename Rule::result_type...>;
            __type &operator=(value_type &&v) noexcept
            {
                this->value = std::move(v);
                return *this;
            }
            value_type value; // NOLINT
        };
        using result_type = __type;

        constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            transaction trans{ctx};
            using value_type = result_type::value_type;
            result_type ret;
            bool success = [&]<std::size_t... I>(std::index_sequence<I...>) noexcept {
                auto apply_one = [&]<std::size_t Idx>() noexcept {
                    if (auto v = this->template get<Idx>().parse(ctx))
                    {
                        ret.value = value_type{*v};
                        return true;
                    }
                    return false;
                };
                return static_cast<bool>((apply_one.template operator()<I>() || ...));
            }(std::make_index_sequence<sizeof...(Rule)>{});
            return success ? (trans.commit(), std::optional<result_type>{std::move(ret)})
                           : std::nullopt;
        }
        static constexpr auto build(const result_type &ctx) noexcept
        {
            return std::visit(
                [](auto &&arg) noexcept -> std::string {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::monostate>)
                    {
                        return {};
                    }
                    else
                    {
                        return T::domain::build(std::forward<decltype(arg)>(arg));
                    }
                },
                ctx.value);
        }
    };

    template <typename... T>
    make_alternative(T &&...r) -> make_alternative<std::decay_t<T>...>;

    template <typename... T>
    inline constexpr bool is_operators_rule<make_alternative<T...>> = true; // NOLINT

}; // namespace mcs::abnf::operators