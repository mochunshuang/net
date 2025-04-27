#pragma once

#include "../__abnf.hpp"
#include <cstdint>

namespace mcs::abnf::uri
{
    // port          = *DIGIT
    struct port
    {
        struct __type
        {
            using domain = port;
            octets_view value;
        };
        using rule_concept = rule_t;
        using result_type = __type;
        using rule = zero_or_more<DIGIT>;

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
        static constexpr auto build(const result_type &ctx)
        {
            std::uint16_t port{};
            return port;
        }
    };

}; // namespace mcs::abnf::uri