#pragma once

#include "../detail/__types.hpp"
#include "./__dec_octet.hpp"
#include <cstddef>
#include <string>

namespace mcs::abnf::uri
{
    // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    struct IPv4address
    {
        struct __type
        {
            using domain = IPv4address;
            detail::absolute_span value;
        };
        using result_type = __type;

        static constexpr auto operator()(detail::const_parser_ctx ctx) noexcept
            -> std::optional<result_type>
        {
            using IPv4address =
                sequence<dec_octet, InsensitiveChar<'.'>, dec_octet, InsensitiveChar<'.'>,
                         dec_octet, InsensitiveChar<'.'>, dec_octet>;
            const size_t k_min_length = 7;
            if (ctx.size() >= k_min_length)
            {
                // TODO(mcs): 自己保留std::span 不更好吗
                if (auto ret = IPv4address{}(ctx))
                    return result_type{
                        .value = {.start = ctx.cur_index, .end = ctx.cur_index + *ret}};
            }

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string IPv4address;
            return IPv4address;
        }
    };

}; // namespace mcs::abnf::uri