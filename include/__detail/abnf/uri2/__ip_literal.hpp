#pragma once

#include "../detail/__types.hpp"
#include "./__ipv6address.hpp"
#include "./__ipvfuture.hpp"
#include <variant>

namespace mcs::abnf::uri
{
    namespace rules
    {
        using IP_literal_rule =
            sequence<CharInsensitive<'['>, alternative<IPv6address, IPvFuture>,
                     CharInsensitive<']'>>;
    };
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    struct IP_literal : SimpleRule<IP_literal, rules::IP_literal_rule>
    {
        struct __type
        {
            using domain = IP_literal;
            using IPv6address_t = IPv6address::result_type;
            using IPvFuture_t = IPvFuture::result_type;
            std::variant<std::monostate, IPv6address_t, IPv6address_t> value;
        };
        using result_type = __type;

        static constexpr auto parse(detail::parser_ctx ctx) -> std::optional<result_type>
        {

            return std::nullopt;
        }
        static constexpr auto build(const result_type &ctx)
        {
            std::string IP_literal;
            return IP_literal;
        }
    };

}; // namespace mcs::abnf::uri