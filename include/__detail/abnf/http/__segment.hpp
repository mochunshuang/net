#pragma once

#include "../uri/__segment.hpp"

namespace mcs::abnf::http
{
    // segment = <segment, see [URI], Section 3.3>
    using segment = uri::segment;
}; // namespace mcs::abnf::http