#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    struct userinfo
    {
        struct __type
        {
            using domain = userinfo;
            octets_view value;
        };
        using rule_concept = rule_t;
        using result_type = __type;
        using rule = zero_or_more<
            alternative<unreserved, pct_encoded, sub_delims, SensitiveChar<':'>>>;

        static constexpr auto operator()(const_parser_ctx ctx) noexcept -> consumed_result
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