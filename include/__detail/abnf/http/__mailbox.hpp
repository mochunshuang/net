#pragma once

#include "../imf/__mailbox.hpp"

namespace mcs::abnf::http
{
    // mailbox = <mailbox, see [RFC5322], Section 3.4>
    using mailbox = imf::mailbox;
}; // namespace mcs::abnf::http
