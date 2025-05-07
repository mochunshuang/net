#pragma once

#include "../__abnf.hpp"
#include "./__ctext.hpp"
#include "./__quoted_pair.hpp"
#include "./__fws.hpp"

namespace mcs::abnf::imf
{
    // ccontent        =   ctext / quoted-pair / comment
    // comment         =   "(" *([FWS] ccontent) [FWS] ")"
    struct ccontent
    {
        struct __type
        {
            using domain = ccontent;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result;
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

    struct comment
    {
        struct __type
        {
            using domain = comment;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result;
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

    constexpr auto ccontent::operator()(parser_ctx_ref ctx) noexcept -> consumed_result
    {
        // ccontent        =   ctext / quoted-pair / comment
        using rule = alternative<ctext, quoted_pair, comment>;
        constexpr auto k_rule = rule{};
        auto ret = k_rule(ctx);
        return ret ? make_consumed_result(*ret) : std::nullopt;
    }
    constexpr auto comment::operator()(parser_ctx_ref ctx) noexcept -> consumed_result
    {
        // comment         =   "(" *([FWS] ccontent) [FWS] ")"
        using rule = sequence<Char<'('>, zero_or_more<sequence<optional<FWS>, ccontent>>,
                              Char<')'>>;
        constexpr auto k_rule = rule{};
        auto ret = k_rule(ctx);
        return ret ? make_consumed_result(*ret) : std::nullopt;
    }

}; // namespace mcs::abnf::imf