#pragma once

#include "./__media_range.hpp"
#include "./__weight.hpp"
#include "./__ows.hpp"
#include "./__parameter_name.hpp"
#include "__parameter_value.hpp"
#include "__type.hpp"
#include <cstddef>

namespace mcs::abnf::http
{
    /**
     *Accept = [ ( media-range [ weight ] ) *( OWS "," OWS ( media-range [
                                                             weight ] ) ) ]
     *
     */
    // using Accept = optional<sequence<
    //     media_range, optional<weight>,
    //     zero_or_more<sequence<OWS, Char<','>, OWS, media_range, optional<weight>>>>>;
    // weight = OWS ";" OWS "q=" qvalue
    // media-range = ( "*/*" / ( type "/*" ) / ( type "/" subtype ) ) parameters
    // parameters = *( OWS ";" OWS [ parameter ] )
    // parameter = parameter-name "=" parameter-value

    struct AcceptRule;
    using Accept = AcceptRule;
    // Accept = [ ( media-range [ weight ] ) *( OWS "," OWS ( media-range [ weight ] ) ) ]
    struct AcceptRule
    {
        struct __type
        {
            using domain = AcceptRule;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            if (ctx.remain() < 3)
                return 0;
            auto old_index = ctx.cur_index;
            size_t count = 0;
            auto update_ctx = [&]() noexcept {
                ctx.cur_index = old_index + count;
            };

            // media-range = ( "*/*" / ( type "/*" ) / ( type "/" subtype ) ) parameters
            auto media_range_check = [&] noexcept -> int {
                // ( "*/*" / ( type "/*" ) / ( type "/" subtype ) )
                if (tool::equal_value(ctx.root_span.subspan(ctx.cur_index, 3), "*/*"))
                {
                    count += 3;
                    update_ctx();
                }
                else
                {
                    const auto k_media_range_start_count = count;

                    const auto k_index =
                        find_char(ctx, {.start = ctx.cur_index, .c = '/'});
                    if (k_index == ctx.end_index)
                        return -1;

                    const auto k_gap = k_index - ctx.cur_index;
                    auto type_ctx =
                        make_parser_ctx(ctx.root_span.subspan(ctx.cur_index, k_gap));
                    constexpr auto k_type_rule = type{};
                    if (not k_type_rule(type_ctx))
                        return -1;

                    count += 1 + k_gap;
                    update_ctx();

                    // check subtype
                    // NOTE: '*' belong subtype == token
                    constexpr auto k_subtype_rule = subtype{};
                    auto ret = k_subtype_rule(ctx);
                    if (not ret.has_value())
                    {
                        count = k_media_range_start_count;
                        update_ctx();
                        return -1;
                    }
                    count += *ret;
                    update_ctx();
                }
                // parameters = *( OWS ";" OWS [ parameter ] )
                // parameter = parameter-name "=" parameter-value
                // weight = OWS ";" OWS "q=" qvalue
                auto parameters_check = [&] noexcept -> bool {
                    while (ctx.remain() > 0)
                    {
                        const auto k_parameters_start_count = count;

                        count += ows_count(ctx);
                        update_ctx();
                        if (ctx.root_span[ctx.cur_index] != ';')
                        {
                            count = k_parameters_start_count;
                            update_ctx();
                            break;
                        }
                        count += 1;
                        update_ctx();
                        count += ows_count(ctx);
                        update_ctx();

                        // [ parameter ]
                        size_t index = ctx.end_index;
                        for (size_t i{ctx.cur_index}; i < ctx.end_index; ++i)
                        {
                            const auto k_cur = ctx.root_span[i];
                            if (k_cur == ';' || k_cur == abnf::sp_value ||
                                k_cur == abnf::htab_value)
                                break;
                            if (k_cur == '=')
                            {
                                index = i;
                                break;
                            }
                        }
                        if (index != ctx.end_index)
                        {
                            const auto k_gap = index - ctx.cur_index;
                            const auto k_parameter_name_span =
                                ctx.root_span.subspan(ctx.cur_index, k_gap);

                            // parameter_name cant not be "q"
                            if (tool::equal_value(k_parameter_name_span, "q") ||
                                tool::equal_value(k_parameter_name_span, "Q"))
                            {
                                count = k_parameters_start_count;
                                update_ctx();
                                break;
                            }

                            constexpr auto k_parameter_name_rule = parameter_name{};
                            auto parameter_name_ctx =
                                make_parser_ctx(k_parameter_name_span);
                            if (not k_parameter_name_rule(parameter_name_ctx))
                                return false;

                            count += 1 + k_gap;
                            update_ctx();
                            if (ctx.remain() == 0)
                                return false;

                            // must parameter_value check true
                            // parameter-value = ( token / quoted-string )
                            bool has_parameter_value = false;
                            constexpr auto k_token_rule = token{};
                            if (auto ret = k_token_rule(ctx))
                            {
                                count += *ret;
                                update_ctx();
                                has_parameter_value = true;
                            }
                            if (not has_parameter_value)
                            {
                                constexpr auto k_quoted_string_rule = quoted_string{};
                                auto ret = k_quoted_string_rule(ctx);
                                if (not ret)
                                    return false;
                                count += *ret;
                                update_ctx();
                            }
                        }
                    }
                    return true;
                };
                if (not parameters_check())
                    return -1;

                return 1;
            };
            if (media_range_check() == -1)
                return count;
            // [ weight ]
            // weight = OWS ";" OWS "q=" qvalue
            auto weight_check = [&] noexcept {
                if (ctx.remain() > 0)
                {
                    const auto k_weight_start_count = count;

                    count += ows_count(ctx);
                    update_ctx();

                    if (ctx.root_span[ctx.cur_index] != ';')
                    {
                        count = k_weight_start_count;
                        update_ctx();
                        return;
                    }
                    count += 1;
                    update_ctx();

                    count += ows_count(ctx);
                    update_ctx();

                    if (ctx.remain() > 2 &&
                        (ctx.root_span[ctx.cur_index] == 'q' ||
                         ctx.root_span[ctx.cur_index] == 'Q') &&
                        ctx.root_span[ctx.cur_index + 1] == '=')
                    {
                        count += 2;
                        update_ctx();
                        // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
                        constexpr auto k_qvalue_rule = qvalue{};
                        auto ret = k_qvalue_rule(ctx);
                        if (not ret)
                        {
                            count = k_weight_start_count;
                            update_ctx();
                            return;
                        }
                        count += *ret;
                        update_ctx();
                    }
                    else
                    {
                        count = k_weight_start_count;
                        update_ctx();
                    }
                }
                return;
            };
            weight_check();

            // *( OWS "," OWS ( media-range [ weight ] ) )
            while (ctx.remain() > 0)
            {
                const auto k_start_count = count;

                count += ows_count(ctx);
                update_ctx();
                if (ctx.remain() > 0 && ctx.root_span[ctx.cur_index] != ',')
                {
                    count = k_start_count;
                    update_ctx();
                    break;
                }
                count += 1;
                update_ctx();

                count += ows_count(ctx);
                update_ctx();

                // media_range_check
                if (ctx.remain() > 0 && media_range_check() == -1)
                {
                    count = k_start_count;
                    update_ctx();
                    break;
                }
                // weight_check
                weight_check();
            }

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