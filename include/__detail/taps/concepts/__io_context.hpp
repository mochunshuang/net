#pragma once

#include "./__server_context.hpp"
#include "./__client_context.hpp"

namespace mcs::taps::concepts
{
    template <typename Context>
    concept io_context = (server_context<Context> || client_context<Context>);

}; // namespace mcs::taps::concepts