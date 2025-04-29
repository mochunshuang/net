#pragma once

#include "../__abnf.hpp"
#include "./__dec_octet.hpp"
#include <cstddef>
#include <string>
#include <utility>

namespace mcs::abnf::uri
{
    // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    struct IPv4address
    {
        struct __type
        {
            using domain = IPv4address;
            octets_view value;
        };
        using result_type = __type;
        using rule_concept = rule_t;
        using rule =
            sequence<dec_octet, CharInsensitive<'.'>, dec_octet, CharInsensitive<'.'>,
                     dec_octet, CharInsensitive<'.'>, dec_octet>;

        static constexpr auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> consumed_result
        {
            if (auto ret = rule{}(ctx))
                return make_consumed_result(*ret);
            return std::nullopt;
        }
        static constexpr auto parse(detail::parser_ctx &ctx) noexcept
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