
#pragma once

#include <cstdint>

namespace mcs::net::services::http
{
    enum parse_state : std::uint8_t
    {
        START_LINE,
        HEADERS,
        BODY,
        COMPLETE
    };
}; // namespace mcs::net::services::http