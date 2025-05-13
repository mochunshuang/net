#pragma once

#include "./__request_line.hpp"
#include "./__status_line.hpp"

namespace mcs::protocol::http::rules
{
    // start-line = request-line / status-line
    using start_line = abnf::alternative<request_line, status_line>;
}; // namespace mcs::protocol::http::rules