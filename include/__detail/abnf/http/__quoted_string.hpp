#pragma once

#include "./__qdtext.hpp"
#include "./__quoted_pair.hpp"

namespace mcs::abnf::http
{
    // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
    using quoted_string =
        sequence<DQUOTE, zero_or_more<alternative<qdtext, quoted_pair>>, DQUOTE>;

#if false // NOLINT //NOTE: DQUOTE qdtext quoted_pair 这3部分没有重叠。没必要手动实现
    struct quoted_string_impl;
    // using quoted_string = quoted_string_impl;
    struct quoted_string_impl
    {
        struct __type
        {
            using domain = quoted_string_impl;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
            if (ctx.remain() < 1 || ctx.root_span[ctx.cur_index] != abnf::dquote_value)
                return std::nullopt;
            auto old_index = ctx.cur_index;
            /**
             * @brief
              using qdtext = alternative<HTAB, SP, Char<'!'>, Range<0x23, 0x5B>,
                             Range<0x5D, 0x7E>, obs_text>;
              using obs_text = Range<0x80, 0xFF>;
              using quoted_pair = sequence<Char<'\\'>, alternative<HTAB, SP, VCHAR,
             obs_text>>;
              //  VCHAR          =  %x21-7E;
             */
            ++(ctx.cur_index);
            constexpr auto k_qdtext_rule = [](auto cur) noexcept {
                return cur == htab_value || cur == sp_value || cur == '!' ||
                       (0x23 <= cur && cur <= 0x5B) || // NOLINT
                       (0x5D <= cur && cur <= 0x7E) || // NOLINT
                       (0x80 <= cur && cur <= 0xFF);   // NOLINT obs_text
            };
            constexpr auto k_quoted_second = [](auto second) noexcept {
                return second == htab_value || second == sp_value ||
                       (0x21 <= second && second <= 0x7E); // NOLINT
            };
            while (true)
            {
                auto remain = ctx.remain();
                // quoted_pair
                if (remain > 1 && ctx.root_span[ctx.cur_index] == '\\' &&
                    k_quoted_second(ctx.root_span[ctx.cur_index + 1]))
                {
                    ctx.cur_index += 2;
                    continue;
                }
                if (remain > 0 && k_qdtext_rule(ctx.root_span[ctx.cur_index]))
                {
                    ++(ctx.cur_index);
                    continue;
                }
                break;
            }
            // NOTE: must end by dquote_value
            if (ctx.remain() > 0 && ctx.root_span[ctx.cur_index] != abnf::dquote_value)
            {
                ctx.cur_index = old_index;
                return std::nullopt;
            }
            ++(ctx.cur_index);
            return ctx.cur_index - old_index;
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
    };
#endif
}; // namespace mcs::abnf::http