#pragma once

#include "./__IPv6address.hpp"
#include "./__IPvFuture.hpp"

namespace mcs::ABNF::URI
{
    // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
    constexpr bool IP_literal(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 4)
            return false;

        //

        return true;
    }
}; // namespace mcs::ABNF::URI