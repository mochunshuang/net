#pragma once

#include "./__cfws.hpp"
#include "./__mailbox.hpp"

namespace mcs::abnf::imf
{
    // obs-mbox-list   =   *([CFWS] ",") mailbox *("," [mailbox / CFWS])
    using obs_mbox_list =
        sequence<zero_or_more<sequence<optional<CFWS>, Char<','>>>, mailbox,
                 zero_or_more<sequence<Char<','>, optional<alternative<mailbox, CFWS>>>>>;
}; // namespace mcs::abnf::imf