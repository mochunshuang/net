#pragma once

#include "./__pchars.hpp"

namespace mcs::ABNF::URI
{
    // segment       = *pchar
    constexpr CheckResult segment(default_span_t sp) noexcept
    {
        return pchars(sp);
    }
}; // namespace mcs::ABNF::URI