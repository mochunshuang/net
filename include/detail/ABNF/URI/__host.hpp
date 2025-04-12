#pragma once

#include "./__IP_literal.hpp"
#include "./__IPv4address.hpp"
#include "./__reg_name.hpp"

namespace mcs::ABNF::URI
{
    //  host          = IP-literal / IPv4address / reg-name
    constexpr bool host(default_span_t sp) noexcept
    {
        return IP_literal(sp) || IPv4address(sp) || reg_name(sp);
    }
}; // namespace mcs::ABNF::URI