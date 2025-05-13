#pragma once

#include "../../../abnf/http/__absolute_path.hpp"

namespace mcs::protocol::http::rules
{
    // absolute-path = <absolute-path, see [HTTP], Section 4.1>
    using absolute_path = ::mcs::abnf::http::absolute_path;
}; // namespace mcs::protocol::http::rules