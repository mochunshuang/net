#pragma once

#include "./__char.hpp"
#include "../operators/__alternative.hpp"
#include <bitset>

namespace mcs::abnf::generate
{
    template <detail::octet... C>
    struct any_of
    {
        struct __type
        {
            using domain = any_of;
            detail::octets_view value;
        };
        using result_type = __type;
        using rule_concept = detail::rule_t;

        static constexpr auto k_bitset = []() consteval {
            std::bitset<std::numeric_limits<detail::octet>::max() + 1> bs;
            for (auto ch : {C...})
                bs.set(ch);
            return bs;
        }();

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            return !ctx.done() && k_bitset[ctx.root_span[ctx.cur_index]]
                       ? (ctx.cur_index += 1, detail::make_consumed_result(1))
                       : std::nullopt;
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
    template <detail::octet... C>
        requires(sizeof...(C) <= 3)
    struct any_of<C...> : operators::alternative<CharSensitive<C>...>
    {
    };

}; // namespace mcs::abnf::generate