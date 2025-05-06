#pragma once

#include "../detail/__types.hpp"
#include <string>

namespace mcs::abnf::generate
{
    template <detail::octet begin, detail::octet end>
    struct Range
    {
        struct __type
        {
            using domain = Range;
            detail::octets_view value;
        };
        using result_type = __type;
        using rule_concept = detail::rule_t;

        constexpr static auto operator()(detail::parser_ctx &ctx) noexcept
            -> detail::consumed_result
        {
            return !ctx.done() && (begin <= ctx.root_span[ctx.cur_index] &&
                                   ctx.root_span[ctx.cur_index] <= end)
                       ? (ctx.cur_index += 1, detail::make_consumed_result(1))
                       : std::nullopt;
        }
        static constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto start{ctx.cur_index};
            auto ret = operator()(ctx);
            return ret ? std::make_optional(
                             result_type{.value = ctx.root_span.subspan(start, *ret)})
                       : std::nullopt;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
    };

}; // namespace mcs::abnf::generate