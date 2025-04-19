#pragma once

#include "../uri/__relative_part.hpp"

namespace mcs::abnf::http
{
    // relative-part = <relative-part, see [URI], Section 4.2>
    constexpr abnf_result auto relative_part(span_param_in sp) noexcept
    {
        return uri::relative_part(sp);
    }
}; // namespace mcs::abnf::http