#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::uri
{
    // unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    constexpr bool unreserved(octet_param_in c) noexcept
    {
        return ALPHA(c) || DIGIT(c) || (c == '-' || c == '.' || c == '_' || c == '~');
    }
}; // namespace mcs::abnf::uri