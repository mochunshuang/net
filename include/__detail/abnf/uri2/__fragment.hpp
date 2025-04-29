#pragma once

#include "./__pchar.hpp"
#include <string>

namespace mcs::abnf::uri
{
    // fragment      = *( pchar / "/" / "?" )
    struct fragment
    {
        struct __type
        {
            using domain = fragment;
            detail::octets_view value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {
            using fragment =
                zero_or_more<alternative<pchar, CharSensitive<'/'>, CharSensitive<'?'>>>;
            if (auto ret = fragment{}(ctx))
                return result_type{.value = ctx.root_span.subspan(ctx.cur_index, *ret)};
            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
    };

}; // namespace mcs::abnf::uri