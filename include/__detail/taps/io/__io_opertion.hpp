#pragma once

#include "./__io_type.hpp"
#include <cstdint>

namespace mcs::net::io
{
    template <typename socket_type>
    struct io_opertion
    {
        io_type type;
        socket_type socket;
        char *buf;
        std::uint32_t len;
    };

}; // namespace mcs::net::io