#pragma once

#include "../../../abnf/http/__absolute_uri.hpp"

namespace mcs::protocol::http::rules
{
    // absolute-URI = <absolute-URI, see [URI], Section 4.3>
    using absolute_URI = ::mcs::abnf::http::absolute_URI;
}; // namespace mcs::protocol::http::rules