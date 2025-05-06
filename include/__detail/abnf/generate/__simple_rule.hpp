#pragma once

#include "../detail/__types.hpp"
#include "../operators/__operators_rule.hpp"
#include <string>

namespace mcs::abnf::generate
{
    template <typename Domain, operators::operators_rule Rule>
    struct SimpleRule
    {
      private:
        SimpleRule() = default;

      public:
        using rule_concept = detail::rule_t;

        struct __type
        {
            using domain = Domain;
            detail::octets_view value;
        };
        using result_type = __type;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            auto ret = Rule{}(ctx);
            return ret ? detail::make_consumed_result(*ret) : std::nullopt;
        }
        static constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto begin{ctx.cur_index};
            auto ret = operator()(ctx);
            return ret ? std::make_optional(
                             result_type{.value = ctx.root_span.subspan(begin, *ret)})
                       : std::nullopt;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
        friend Domain;
    };

}; // namespace mcs::abnf::generate