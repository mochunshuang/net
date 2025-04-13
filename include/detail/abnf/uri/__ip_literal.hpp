#pragma once

#include "./__ipv6address.hpp"
#include "./__ipvfuture.hpp"

namespace mcs::abnf::uri
{
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    constexpr bool IP_literal(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 4)
            return false;

        if (sp[0] != '[' || sp[k_size - 1] != ']')
            return false;

        const auto k_check_span = sp.subspan(1, k_size - 2);
        return IPv6address(k_check_span) || IPvFuture(k_check_span);
    }
}; // namespace mcs::abnf::uri