#pragma once

#include "./__segment.hpp"
#include "./__segment_nz.hpp"
#include <optional>

namespace mcs::abnf::uri
{
    // path-rootless = segment-nz *( "/" segment )
    struct path_rootless
    {
        struct __type
        {
            using domain = path_rootless;
            using segment_t = segment::result_type;
            using segment_nz_t = segment_nz::result_type;

            octets_view value;
        };
        using rule_concept = rule_t;
        using result_type = __type;
        using rule =
            sequence<segment_nz, zero_or_more<sequence<CharSensitive<'/'>, segment>>>;

        static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
        {
            if (auto ret = rule{}(ctx))
                return make_consumed_result(*ret);
            return std::nullopt;
        }
        static constexpr auto parse(parser_ctx &ctx) noexcept
            -> std::optional<result_type>
        {
            if (auto ret = operator()(ctx))
                return result_type{
                    .value = ctx.root_span.subspan(
                        std::exchange(ctx.cur_index, ctx.cur_index + *ret), *ret)};
            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
    };

}; // namespace mcs::abnf::uri