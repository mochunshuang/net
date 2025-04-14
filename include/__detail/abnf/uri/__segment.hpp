#pragma once

#include "./__pchars.hpp"

namespace mcs::abnf::uri
{
    // segment       = *pchar
    constexpr auto segment(span_param_in sp) noexcept -> abnf_result auto
    {
        return pchars(sp);
    }
}; // namespace mcs::abnf::uri