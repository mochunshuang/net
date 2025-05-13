#pragma once

#include "../../../abnf/http/__rws.hpp"

namespace mcs::protocol::http::rules
{
    // RWS = <RWS, see [HTTP], Section 5.6.3>
    using RWS = abnf::http::RWS;
}; // namespace mcs::protocol::http::rules