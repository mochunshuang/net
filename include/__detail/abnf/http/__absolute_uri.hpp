#pragma once

#include "../uri/__absolute_uri.hpp"

namespace mcs::abnf::http
{
    // absolute-URI = <absolute-URI, see [URI], Section 4.3>
    using absolute_URI = uri::absolute_URI;
}; // namespace mcs::abnf::http