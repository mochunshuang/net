#pragma once

#include "../uri/__segment.hpp"

namespace mcs::abnf::http
{
    // segment = <segment, see [URI], Section 3.3>
    constexpr abnf_result auto segment(span_param_in sp) noexcept
    {
        return uri::segment(sp);
    }
}; // namespace mcs::abnf::http