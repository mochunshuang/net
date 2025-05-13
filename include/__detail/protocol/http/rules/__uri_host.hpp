#pragma once

#include "../../../abnf/http/__uri_host.hpp"

namespace mcs::protocol::http::rules
{
    // uri-host = <host, see [URI], Section 3.2.2>
    using uri_host = ::mcs::abnf::http::uri_host;
}; // namespace mcs::protocol::http::rules