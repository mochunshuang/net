#pragma once

#include "./__ip_literal.hpp"
#include "./__ipv4address.hpp"
#include "./__reg_name.hpp"

namespace mcs::abnf::uri
{
    //  host          = IP-literal / IPv4address / reg-name
    constexpr bool host(span_param_in sp) noexcept
    {
        return IP_literal(sp) || IPv4address(sp) || reg_name(sp);
    }
}; // namespace mcs::abnf::uri