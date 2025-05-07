#pragma once

#include "../uri/__host.hpp"

namespace mcs::abnf::http
{
    // uri-host = <host, see [URI], Section 3.2.2>
    using uri_host = uri::host;
}; // namespace mcs::abnf::http