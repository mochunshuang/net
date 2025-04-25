#pragma once

#include "../detail/__types.hpp"
#include "./__ip_literal.hpp"
#include "./__ipv4address.hpp"
#include "./__reg_name.hpp"
#include <algorithm>
#include <variant>

namespace mcs::abnf::uri
{
    //  host          = IP-literal / IPv4address / reg-name
    struct host
    {
        struct __type
        {
            using domain = host;
            using IP_literal_t = IP_literal::result_type;
            using IPv4address_t = IPv4address::result_type;
            using reg_name_t = reg_name::result_type;

            std::variant<std::monostate, IP_literal_t, IPv4address_t, reg_name_t> value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string host;
            return host;
        }
    };

}; // namespace mcs::abnf::uri