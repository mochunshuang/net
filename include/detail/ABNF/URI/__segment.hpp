#pragma once

#include "./__segment_nz.hpp"

namespace mcs::ABNF::URI
{
    // segment       = *pchar
    constexpr bool segment(default_span_t sp) noexcept
    {
        if (sp.empty())
            return true;
        return segment_nz(sp);
    }
}; // namespace mcs::ABNF::URI