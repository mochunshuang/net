#pragma once

#include "./__cfws.hpp"
#include "./__mailbox.hpp"

namespace mcs::abnf::imf
{
    // obs-mbox-list   =   *([CFWS] ",") mailbox *("," [mailbox / CFWS])
    using obs_mbox_list = sequence<
        zero_or_more<sequence<optional<CFWS>, CharSensitive<','>>>, mailbox,
        zero_or_more<sequence<CharSensitive<','>, optional<alternative<mailbox, CFWS>>>>>;
}; // namespace mcs::abnf::imf