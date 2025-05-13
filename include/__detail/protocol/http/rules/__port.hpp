#pragma once

#include "../../../abnf/http/__port.hpp"

namespace mcs::protocol::http::rules
{
    // port = <port, see [URI], Section 3.2.3>
    using port = abnf::http::port;
}; // namespace mcs::protocol::http::rules