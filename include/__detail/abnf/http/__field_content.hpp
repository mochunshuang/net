#pragma once

#include "./__field_vchar.hpp"

namespace mcs::abnf::http
{
    /**
     * field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar ]
     *
     */
    // using field_content = sequence<
    //     field_vchar,
    //     optional<sequence<one_or_more<alternative<SP, HTAB, field_vchar>>,
    //     field_vchar>>>;
    struct field_content_impl;
    using field_content = field_content_impl;
    struct field_content_impl
    {
        struct __type
        {
            using domain = field_content_impl;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            // field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar ]
            if (ctx.remain() < 1)
                return std::nullopt;
            auto old_index = ctx.cur_index;
            size_t count = 0;
            auto update_ctx = [&]() noexcept {
                ctx.cur_index = old_index + count;
            };
            constexpr auto k_field_vchar_rule = field_vchar{};
            auto ret = k_field_vchar_rule(ctx);
            if (not ret)
                return std::nullopt;
            count += *ret;
            update_ctx();

            // [ 1*( SP / HTAB / field-vchar ) field-vchar ]
            size_t optional_count = 0;
            bool last_pass_is_field_vchar = false;
            if (ctx.remain() > 1)
            {
                if (auto new_ctx = ctx; // NOTE: bacause xxx_rule change ctx status
                    ctx.root_span[ctx.cur_index] == abnf::sp_value ||
                    ctx.root_span[ctx.cur_index] == abnf::htab_value ||
                    k_field_vchar_rule(new_ctx)) // first check
                {
                    ++optional_count;
                    ++(ctx.cur_index);

                    auto sp_htab_field_vchar_rule = [&](auto ctx) noexcept {
                        if (ctx.root_span[ctx.cur_index] == abnf::sp_value ||
                            ctx.root_span[ctx.cur_index] == abnf::htab_value)
                        {
                            last_pass_is_field_vchar = false;
                            return true;
                        }
                        if (k_field_vchar_rule(ctx))
                        {
                            last_pass_is_field_vchar = true;
                            return true;
                        }
                        return false;
                    };
                    while (ctx.remain() > 0 && sp_htab_field_vchar_rule(ctx))
                    {
                        ++optional_count;
                        ++(ctx.cur_index);
                    }
                }
            }
            // NOTE: last must by field-vchar pass
            if (last_pass_is_field_vchar)
                count += optional_count;
            else
                count += 0;
            update_ctx();
            return count;
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
}; // namespace mcs::abnf::http