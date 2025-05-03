#pragma once

#include "../detail/__types.hpp"
#include "__product_type.hpp"
#include <string>

namespace mcs::abnf::operators
{
    template <typename Rule>
    struct CharRule : product_type<Rule>
    {
        using rule_concept = detail::rule_t;

        struct __type
        {
            using domain = CharRule;
            detail::octets_view value;
        };
        using result_type = __type;

        constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            auto &rule = this->template get<0>();
            auto ret = rule(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }
        constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto begin{ctx.cur_index};
            auto ret = operator()(ctx);
            return ret ? std::make_optional(
                             result_type{.value = ctx.root_span.subspan(begin, *ret)})
                       : std::nullopt;
        }
        static constexpr auto build(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
    };

}; // namespace mcs::abnf::operators