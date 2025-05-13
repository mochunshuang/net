#pragma once

#include "../../../abnf/http/__bws.hpp"

namespace mcs::protocol::http::rules
{
    // BWS = <BWS, see [HTTP], Section 5.6.3>
    using BWS = abnf::http::BWS;
}; // namespace mcs::protocol::http::rules