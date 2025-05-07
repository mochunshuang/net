#pragma once

#include "./__ctext.hpp"
#include "./__quoted_pair.hpp"

namespace mcs::abnf::http
{
    // comment = "(" *( ctext / quoted-pair / comment ) ")"
    struct comment
    {
        using rule_concept = rule_t;
        struct __type
        {
            using domain = comment;
            octets_view value;
        };
        using result_type = __type;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            using Rule = sequence<Char<'('>,
                                  zero_or_more<alternative<ctext, quoted_pair, comment>>,
                                  Char<')'>>;
            constexpr auto k_rule = Rule{};
            auto ret = k_rule(ctx);
            return ret ? make_consumed_result(*ret) : std::nullopt;
        }
        static constexpr auto parse(parser_ctx_ref ctx) noexcept
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

}; // namespace mcs::abnf::http