#pragma once

#include "../uri/__relative_part.hpp"

namespace mcs::abnf::http
{
    // relative-part = <relative-part, see [URI], Section 4.2>
    using relative_part = uri::relative_part;
}; // namespace mcs::abnf::http