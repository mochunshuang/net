#pragma once

#include "./__io_context.hpp"

namespace mcs::net::io::windows
{
    struct listener
    {
        io_context &io_ctx; // NOLINT
    };

}; // namespace mcs::net::io::windows