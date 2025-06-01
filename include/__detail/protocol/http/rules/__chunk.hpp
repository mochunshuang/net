#pragma once

#include "./__chunk_data.hpp"
#include "./__chunk_ext.hpp"
#include "./__chunk_size.hpp"

namespace mcs::protocol::http::rules
{
    // chunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF
    // using chunk = abnf::sequence<chunk_size, abnf::optional<chunk_ext>, abnf::CRLF,
    //                              chunk_data, abnf::CRLF>;

    struct chunk_impl;
    using chunk = chunk_impl;
    struct chunk_impl
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
            // chunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF
            if (ctx.remain() < 5) // NOLINT
                return std::nullopt;
            auto old_index = ctx.cur_index;

            auto rallback = [&] noexcept {
                ctx.cur_index = old_index;
            };
            // 1. chunk_size
            constexpr auto k_chunk_size_rule = chunk_size{};
            auto ret = k_chunk_size_rule(ctx);
            if (not ret.has_value())
            {
                rallback();
                return std::nullopt;
            }
            // 2. [ chunk-ext ]

            constexpr auto k_chunk_ext_rule = abnf::optional<chunk_ext>{};
            k_chunk_ext_rule(ctx);

            // 3. CRLF chunk-data CRLF
            if (ctx.remain() < 4)
            {
                rallback();
                return std::nullopt;
            }
            if (ctx.root_span[ctx.cur_index] != abnf::core::cr_value ||
                ctx.root_span[ctx.cur_index + 1] != abnf::core::lf_value)
            {
                rallback();
                return std::nullopt;
            }
            ctx.cur_index += 2;

            // chunk-data = 1*OCTET
            if (ctx.remain() == 0)
            {
                rallback();
                return std::nullopt;
            }
            bool last_pass_by_crlf = false;
            while (true)
            {
                auto remain = ctx.remain();
                if (remain == 0)
                    break;
                if (remain > 1 && ctx.root_span[ctx.cur_index] == abnf::core::cr_value &&
                    ctx.root_span[ctx.cur_index + 1] == abnf::core::lf_value)
                {
                    ctx.cur_index += 2;
                    last_pass_by_crlf = true;
                    continue;
                }
                // NOTE: all date are OCTET
                last_pass_by_crlf = false;
                ++(ctx.cur_index);
            }
            if (not last_pass_by_crlf)
            {
                rallback();
                return std::nullopt;
            }
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