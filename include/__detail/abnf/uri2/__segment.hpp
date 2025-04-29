#pragma once

#include "./__pchar.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // segment       = *pchar
    struct segment
    {
        struct __type
        {
            using domain = segment;
            octets_view value;
        };
        using result_type = __type;
        using rule = zero_or_more<pchar>;
        using rule_concept = rule_t;

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