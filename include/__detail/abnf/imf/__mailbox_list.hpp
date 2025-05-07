#pragma once

#include "./__mailbox.hpp"
#include "./__obs_mbox_list.hpp"

namespace mcs::abnf::imf
{
    // mailbox-list    =   (mailbox *("," mailbox)) / obs-mbox-list
    using mailbox_list =
        alternative<sequence<mailbox, zero_or_more<sequence<Char<','>, mailbox>>>,
                    obs_mbox_list>;

}; // namespace mcs::abnf::imf