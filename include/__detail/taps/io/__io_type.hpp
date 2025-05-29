#pragma once

#include <cstdint>

namespace mcs::net::io
{

    enum class io_type : std::uint8_t
    {
        IO_UNKNOWN,
        IO_ACCEPT,
        IO_READ,
        IO_WRITE
    };

}; // namespace mcs::net::io