#pragma once

#include "./__angle_addr.hpp"
#include "./__display_name.hpp"

namespace mcs::abnf::imf
{
    // name-addr       =   [display-name] angle-addr
    using name_addr = sequence<optional<display_name>, angle_addr>;
} // namespace mcs::abnf::imf