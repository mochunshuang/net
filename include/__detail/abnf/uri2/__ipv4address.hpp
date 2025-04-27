#pragma once

#include "../__abnf.hpp"
#include "./__dec_octet.hpp"
#include <cstddef>
#include <cstdint>
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
        using rule =
            sequence<dec_octet, InsensitiveChar<'.'>, dec_octet, InsensitiveChar<'.'>,
                     dec_octet, InsensitiveChar<'.'>, dec_octet>;

        static constexpr auto operator()(detail::const_parser_ctx ctx) noexcept
            -> consumed_result
        {
            const size_t k_min_length = 7;
            if (ctx.size() >= k_min_length)
            {
                if (auto ret = rule{}(ctx))
                    return make_consumed_result(*ret);
            }
            return std::nullopt;
        }
        static constexpr auto parse(detail::parser_ctx &ctx) noexcept
            -> std::optional<result_type>
        {
            struct Callback
            {
                constexpr auto operator()(const_parser_ctx ctx) noexcept
                    -> consumed_result
                {
                    if (index < 3)
                        dots[index++] = ctx.cur_index;
                    return make_consumed_result(0);
                }
                size_t dots[3]{SIZE_MAX};
                size_t index = 0;
            } callback;

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