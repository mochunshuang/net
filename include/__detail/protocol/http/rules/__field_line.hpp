#pragma once

#include "./__field_name.hpp"
#include "./__field_value.hpp"
// #include "./__ows.hpp"
#include <cstddef>
#include <optional>

namespace mcs::protocol::http::rules
{
    // field-line = field-name ":" OWS field-value OWS
    // using field_line = abnf::sequence<field_name, abnf::Char<':'>, OWS, field_value,
    // OWS>;

    struct FieldLineRuleImpl;
    using field_line = FieldLineRuleImpl;

    // field-line = field-name ":" OWS field-value OWS
    struct FieldLineRuleImpl
    {
        struct __type
        {
            using domain = FieldLineRuleImpl;
            abnf::octets_view value;
        };
        using result_type = __type;
        using rule_concept = abnf::rule_t;

        static constexpr auto operator()(abnf::parser_ctx_ref ctx) noexcept
            -> abnf::consumed_result
        {
            if (ctx.remain() < 2)
                return std::nullopt;
            auto old_index = ctx.cur_index;
            size_t count = 0;
            auto update_ctx = [&]() noexcept {
                ctx.cur_index = old_index + count;
            };
            auto rallback = [&] noexcept {
                ctx.cur_index = old_index;
            };

            // field-line = field-name ":" OWS field-value OWS
            // 0. split
            auto const k_index =
                abnf::tool::find_char(ctx, {.start = ctx.cur_index, .c = ':'});
            if (k_index == ctx.end_index)
            {
                rallback();
                return std::nullopt;
            }

            // 1. field-name check
            auto field_name_ctx = abnf::make_parser_ctx(ctx.root_span.first(k_index));
            constexpr auto k_field_name_rule = field_name{};
            auto ret = k_field_name_rule(field_name_ctx);
            if (not ret.has_value())
            {
                rallback();
                return std::nullopt;
            }

            count += *ret;
            update_ctx();

            // 2. skip ':'
            count += +1;
            update_ctx();

            // 3.1. OWS filter
            count += abnf::tool::ows_count(ctx);
            update_ctx();

            // 3.2. field_value
            constexpr auto k_field_value_rule = field_value{};
            ret = k_field_value_rule(ctx);
            if (not ret.has_value())
            {
                rallback();
                return std::nullopt;
            }

            count += *ret;
            update_ctx();

            // 3.3. OWS filter
            count += abnf::tool::ows_count(ctx);
            update_ctx();

            return count;
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