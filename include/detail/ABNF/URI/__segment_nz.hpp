#pragma once

#include "./__pchars.hpp"

namespace mcs::ABNF::URI
{
    // segment-nz    = 1*pchar
    constexpr CheckResult segment_nz(default_span_t sp) noexcept
    {
        if (sp.empty())
            return Fail(0);
        return pchars(sp);
    }
}; // namespace mcs::ABNF::URI