#pragma once

#include "./__pchars.hpp"

namespace mcs::abnf::uri
{
    // segment-nz    = 1*pchar
    constexpr CheckResult segment_nz(span_param_in sp) noexcept
    {
        if (sp.empty())
            return Fail(0);
        return pchars(sp);
    }
}; // namespace mcs::abnf::uri