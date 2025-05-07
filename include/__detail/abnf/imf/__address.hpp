#pragma once

#include "./__group.hpp"
#include "./__mailbox.hpp"

namespace mcs::abnf::imf
{
    // address         =   mailbox / group
    using address = alternative<mailbox, group>;
}; // namespace mcs::abnf::imf