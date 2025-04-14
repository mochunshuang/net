#pragma once

#include "./__pchars.hpp"

namespace mcs::abnf::uri
{
    // segment       = *pchar
    constexpr CheckResult segment(span_param_in sp) noexcept
    {
        return pchars(sp);
    }
}; // namespace mcs::abnf::uri