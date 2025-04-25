#pragma once

#include "../detail/__types.hpp"
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

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string IPv4address;
            return IPv4address;
        }
    };

}; // namespace mcs::abnf::uri