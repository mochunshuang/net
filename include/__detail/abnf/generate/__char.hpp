#pragma once

#include "../detail/__types.hpp"
#include "../tool/__to_upper.hpp"
#include "../tool/__to_lower.hpp"
#include <string>

namespace mcs::abnf::generate
{
    template <detail::octet C, bool Insensitive>
    struct Char
    {
        struct __type
        {
            using domain = Char;
            detail::octets_view value;
        };
        using rule_concept = detail::rule_t;
        using result_type = __type;

        constexpr static auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            if constexpr (Insensitive)
            {
                return !ctx.empty() &&
                               (tool::to_upper(ctx.root_span[ctx.cur_index]) == C ||
                                tool::to_lower(ctx.root_span[ctx.cur_index]) == C)
                           ? (ctx.cur_index++, detail::make_consumed_result(1))
                           : std::nullopt;
            }
            else
            {
                return !ctx.empty() && ctx.root_span[ctx.cur_index] == C
                           ? (ctx.cur_index++, detail::make_consumed_result(1))
                           : std::nullopt;
            }
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
    };

    template <detail::octet C>
    using CharInsensitive = Char<C, true>;

    template <detail::octet C>
    using CharSensitive = Char<C, false>;

}; // namespace mcs::abnf::generate