#pragma once

#include "./__script.hpp"
#include "./__region.hpp"
#include "./__alphanum.hpp"
#include "./__singleton.hpp"
#include <cstddef>

namespace mcs::abnf::tfil
{
    /**
     *  extlang       = 3ALPHA              ; selected ISO 639 codes
     *                  *2("-" 3ALPHA)      ; permanently reserved
     *
     */
    /**
     * language   = 2*3ALPHA            ; shortest ISO 639 code
                    ["-" extlang]       ; sometimes followed by
                                     ; extended language subtags
                    / 4ALPHA              ; or reserved for future use
                    / 5*8ALPHA            ; or registered language subtag
     *
     */
    //  script        = 4ALPHA              ; ISO 15924 code
    /**
     * region        = 2ALPHA              ; ISO 3166-1 code
                     / 3DIGIT              ; UN M.49 code
     *
     */
    /**
     * variant       = 5*8alphanum         ; registered variants
                     / (DIGIT 3alphanum)
     *
     */
    // extension     = singleton 1*("-" (2*8alphanum))
    /**
     *  langtag       = language
                 ["-" script]
                 ["-" region]
                 *("-" variant)
                 *("-" extension)
                 ["-" privateuse]
     *
     */
    // using langtag = // NOTE: 重构或限制
    //     sequence<
    //         repetition<2, 3, ALPHA>,

    //         alternative<optional<sequence<Char<'-'>, script>>,
    //                     optional<repetition<0, 3, sequence<Char<'-'>, times<3,
    //                     ALPHA>>>>, optional<sequence<Char<'-'>, region>>,
    //                     zero_or_more<sequence<Char<'-'>, variant>>,
    //                     zero_or_more<sequence<Char<'-'>, extension>>,
    //                     optional<sequence<Char<'-'>, privateuse>>>>;
    struct langtag_rule;
    using langtag = langtag_rule;
    struct langtag_rule
    {
        struct __type
        {
            using domain = langtag_rule;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            auto old_index = ctx.cur_index;
            size_t count = 0;
            auto update_ctx = [&]() noexcept {
                ctx.cur_index = old_index + count;
            };
            /**
            * language   = 2*3ALPHA            ; shortest ISO 639 code
                            ["-" extlang]       ; sometimes followed by
                                            ; extended language subtags
                            / 4ALPHA              ; or reserved for future use
                            / 5*8ALPHA            ; or registered language subtag
            *
            */
            while (abnf::ALPHA{}(ctx))
                ++count;
            if (count < 2 || count > 8) // NOLINT
                return 0;
            update_ctx();

            auto find_next_index = [&](size_t start) noexcept {
                for (size_t i = start; i < ctx.end_index; ++i)
                {
                    if (ctx.root_span[i] == '-')
                        return i;
                }
                return ctx.end_index;
            };
            if (count == 2 || count == 3)
            {
                /**
                 *  extlang       = 3ALPHA              ; selected ISO 639 codes
                 *                  *2("-" 3ALPHA)      ; permanently reserved
                 *
                 */
                int max_repetition = 3;
                while (max_repetition != 0)
                {
                    if (ctx.done() || ctx.root_span[ctx.cur_index] != '-')
                        break;
                    auto start = ctx.cur_index + 1;
                    auto end = find_next_index(start);
                    if (end - start != 3)
                        break;

                    auto span = ctx.root_span.subspan(start, 3);
                    constexpr auto k_extlang_rule = times<3, abnf::ALPHA>{};
                    auto extlang_ctx = make_parser_ctx(span);
                    if (not k_extlang_rule(extlang_ctx))
                        break;

                    ctx.cur_index = end;
                    --max_repetition;

                    count += 4;
                }
                update_ctx();
            }

            // ["-" script]
            //  script        = 4ALPHA              ; ISO 15924 code
            if (ctx.end_index - ctx.cur_index > 4 &&
                ctx.root_span[ctx.cur_index] == '-') // NOLINT
            {
                auto start = ctx.cur_index + 1;
                auto end = find_next_index(start);
                auto gap = end - start;
                if (gap == 4)
                {
                    constexpr auto k_script_rule = script{};
                    auto script_ctx = make_parser_ctx(ctx.root_span.subspan(start, gap));
                    if (k_script_rule(script_ctx))
                        count += 5; // NOLINT
                    update_ctx();
                }
            }

            //["-" region]
            /**
            * region        = 2ALPHA              ; ISO 3166-1 code
                            / 3DIGIT              ; UN M.49 code
            */
            if (ctx.remain() > 2 && ctx.root_span[ctx.cur_index] == '-')
            {
                auto start = ctx.cur_index + 1;
                auto end = find_next_index(start);
                auto gap = end - start;
                if (gap == 2 || gap == 3)
                {
                    constexpr auto k_region_rule = region{};
                    auto region_ctx = make_parser_ctx(ctx.root_span.subspan(start, gap));
                    if (k_region_rule(region_ctx))
                        count += 1 + gap;
                    update_ctx();
                }
            }
            /**
            * variant       = 5*8alphanum         ; registered variants
                            / (DIGIT 3alphanum)
            *
            */
            // *("-" variant)
            while (ctx.end_index - ctx.cur_index > 4 &&
                   ctx.root_span[ctx.cur_index] == '-')
            {

                auto start = ctx.cur_index + 1;
                auto end = find_next_index(start);
                auto gap = end - start;
                if (gap < 4 || gap > 8) // NOLINT
                    break;

                auto variant_ctx = make_parser_ctx(ctx.root_span.subspan(start, gap));
                if (gap == 4)
                {
                    constexpr auto k_variant_rule = sequence<DIGIT, times<3, alphanum>>{};
                    if (k_variant_rule(variant_ctx))
                    {
                        count += 1 + gap;
                        update_ctx();
                        continue;
                    }
                    update_ctx();
                    break;
                }
                constexpr auto k_variant_rule = repetition<5, 8, alphanum>{};
                if (k_variant_rule(variant_ctx))
                {
                    count += 1 + gap;
                    update_ctx();
                    continue;
                }
                update_ctx();
                break;
            }
            /**
            *singleton     = DIGIT               ; 0 - 9
                        / %x41-57             ; A - W
                        / %x59-5A             ; Y - Z
                        / %x61-77             ; a - w
                        / %x79-7A             ; y - z
            */
            //*("-" extension)
            // extension     = singleton 1*("-" (2*8alphanum))
            while (ctx.end_index - ctx.cur_index > 5 && // NOLINT
                   ctx.root_span[ctx.cur_index] == '-')
            {
                auto extension_start_count = count;
                constexpr auto k_singleton_rule = singleton{};

                ++ctx.cur_index;            // skip '-
                if (!k_singleton_rule(ctx)) // singleton check
                {
                    update_ctx();
                    break;
                }
                count += 2;

                // Singleton parsed, now require [at least one segment]
                bool has_segment = false;
                while (ctx.remain() > 2 &&
                       ctx.root_span[ctx.cur_index] == '-') // 1*("-" (2*8alphanum))
                {
                    // Parse 2-8 alphanum characters
                    auto start = ctx.cur_index + 1; // skip '-
                    auto end = find_next_index(start + 1);
                    auto gap = end - start;
                    if (gap < 2 || gap > 8) // NOLINT
                        break;
                    constexpr auto k_alphanum_rule = repetition<2, 8, alphanum>{};
                    auto alphanum_ctx =
                        make_parser_ctx(ctx.root_span.subspan(start, gap));
                    if (!k_alphanum_rule(alphanum_ctx))
                        break;

                    count += 1 + gap;
                    update_ctx();
                    has_segment = true;
                }

                if (not has_segment)
                {
                    count = extension_start_count;
                    update_ctx();
                    break;
                }
            }
            // privateuse    = "x" 1*("-" (1*8alphanum))
            //["-" privateuse]
            if (ctx.end_index - ctx.cur_index > 3 && ctx.root_span[ctx.cur_index] == '-')
            {
                auto privateuse_start_count = count;
                constexpr auto k_privateuse_rule = CharInsensitive<'x'>{};
                ++ctx.cur_index;             // skip '-
                if (!k_privateuse_rule(ctx)) // privateuse check
                {
                    update_ctx();
                }
                else
                {
                    count += 2;
                    bool has_privateuse = false;
                    while (ctx.remain() > 1 &&
                           ctx.root_span[ctx.cur_index] == '-') // 1*("-" (1*8alphanum))
                    {
                        auto start = ctx.cur_index + 1; // skip '-
                        auto end = find_next_index(start + 1);
                        auto gap = end - start;
                        if (gap < 1 || gap > 8) // NOLINT
                            break;
                        constexpr auto k_alphanum_rule = repetition<1, 8, alphanum>{};
                        auto alphanum_ctx =
                            make_parser_ctx(ctx.root_span.subspan(start, gap));
                        if (!k_alphanum_rule(alphanum_ctx))
                            break;

                        count += 1 + gap;
                        update_ctx();
                        has_privateuse = true;
                    }
                    if (not has_privateuse)
                    {
                        count = privateuse_start_count;
                        update_ctx();
                    }
                }
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

}; // namespace mcs::abnf::tfil