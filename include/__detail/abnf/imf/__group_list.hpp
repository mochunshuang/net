#pragma once

#include "./__cfws.hpp"
#include "./__mailbox_list.hpp"
#include "./__obs_group_list.hpp"

namespace mcs::abnf::imf
{
    // group-list      =   mailbox-list / CFWS / obs-group-list
    using group_list = alternative<mailbox_list, CFWS, obs_group_list>;
}; // namespace mcs::abnf::imf