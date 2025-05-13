#pragma once

#include "../../../abnf/http/__ows.hpp"

namespace mcs::protocol::http::rules
{
    // OWS = <OWS, see [HTTP], Section 5.6.3>
    using OWS = ::mcs::abnf::http::OWS;
}; // namespace mcs::protocol::http::rules