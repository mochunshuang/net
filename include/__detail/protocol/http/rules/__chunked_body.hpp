#pragma once

#include "./__chunk.hpp"
#include "./__last_chunk.hpp"
#include "./__trailer_section.hpp"

namespace mcs::protocol::http::rules
{
    // chunked-body = *chunk last-chunk trailer-section CRLF
    // using chunk_body = abnf::sequence<abnf::zero_or_more<chunk>, last_chunk,
    //                                   trailer_section, abnf::CRLF>;
    struct chunk_body_impl;
    using chunk_body = chunk_body_impl;
    // NOTE: chunk 和 last_chunk 重叠 chunk-ext
    struct chunk_body_impl
    {
        struct __type
        {
            using domain = chunk_impl;
            abnf::octets_view value;
        };
        using result_type = __type;
        using rule_concept = abnf::rule_t;

        static constexpr auto operator()(abnf::parser_ctx_ref ctx) noexcept
            -> abnf::consumed_result
        {
            // chunked-body = *chunk last-chunk trailer-section CRLF
            if (ctx.remain() < 5) // NOLINT
                return std::nullopt;
            auto old_index = ctx.cur_index;

            auto rallback = [&] noexcept {
                ctx.cur_index = old_index;
            };
            // chunked-body = *chunk last-chunk trailer-section CRLF
            // chunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF
            // last-chunk = 1*"0" [ chunk-ext ] CRLF
            // 0.  *chunk last-chunk
            while (true) // NOTE: only break by last-chunk is good case
            {
                // 1. chunk_size
                constexpr auto k_chunk_size_rule = chunk_size{};
                auto tmp_index = ctx.cur_index;
                auto ret = k_chunk_size_rule(ctx);
                if (not ret.has_value())
                {
                    rallback();
                    return std::nullopt;
                }
                // ret to size
                auto chunk_size = abnf::tool::hex_span_to_size_value(
                    ctx.root_span.subspan(tmp_index, *ret));
                if (not chunk_size.has_value())
                {
                    rallback();
                    return std::nullopt;
                }

                // 2. [ chunk-ext ]
                constexpr auto k_chunk_ext_rule = abnf::optional<chunk_ext>{};
                k_chunk_ext_rule(ctx);

                // 3. CRLF
                if (ctx.remain() < 2 ||
                    ctx.root_span[ctx.cur_index] != abnf::core::cr_value ||
                    ctx.root_span[ctx.cur_index + 1] != abnf::core::lf_value)
                {
                    rallback();
                    return std::nullopt;
                }
                ctx.cur_index += 2;

                if (*chunk_size == 0) // NOTE: match last-chunk
                    break;
                // 4. chunk-data CRLF
                ctx.cur_index += *chunk_size; // NOTE: 严格检验
                // CRLF
                if (ctx.remain() < 2 ||
                    ctx.root_span[ctx.cur_index] != abnf::core::cr_value ||
                    ctx.root_span[ctx.cur_index + 1] != abnf::core::lf_value)
                {
                    rallback();
                    return std::nullopt;
                }
                ctx.cur_index += 2;
            }

            // trailer-section = *( field-line CRLF )
            // 1. trailer-section
            constexpr auto k_trailer_section_rule = trailer_section{};
            k_trailer_section_rule(ctx);
            // 2. CRLF
            if (ctx.remain() < 2 ||
                ctx.root_span[ctx.cur_index] != abnf::core::cr_value ||
                ctx.root_span[ctx.cur_index + 1] != abnf::core::lf_value)
                return std::nullopt;

            ctx.cur_index += 2;
            return ctx.cur_index - old_index;
        }
        static constexpr auto parse(abnf::parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto begin{ctx.cur_index};
            auto ret = operator()(ctx);
            return ret ? std::make_optional(
                             result_type{.value = ctx.root_span.subspan(begin, *ret)})
                       : std::nullopt;
        }
    };
}; // namespace mcs::protocol::http::rules