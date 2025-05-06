#pragma once

#include "../__abnf.hpp"
#include "./__dec_octet.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using IPv4address_rule =
            sequence<dec_octet, CharInsensitive<'.'>, dec_octet, CharInsensitive<'.'>,
                     dec_octet, CharInsensitive<'.'>, dec_octet>;
    };
    // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    struct IPv4address : SimpleRule<IPv4address, rules::IPv4address_rule>
    {
        using SimpleRule<IPv4address, rules::IPv4address_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri