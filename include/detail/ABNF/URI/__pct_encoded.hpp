#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::uri
{
    // NOTE: Atomicity should be ensured so need 3 parameter
    // pct-encoded   = "%" HEXDIG HEXDIG
    constexpr bool pct_encoded(octet_t a, octet_t b, octet_t c) noexcept
    {
        return a == '%' && HEXDIG(b) && HEXDIG(c);
    }
}; // namespace mcs::abnf::uri