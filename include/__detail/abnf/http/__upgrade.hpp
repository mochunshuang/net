#pragma once

#include "./__protocol.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Upgrade = [ protocol *( OWS "," OWS protocol ) ]
    using Upgrade = optional<
        sequence<protocol, zero_or_more<sequence<OWS, Char<','>, OWS, protocol>>>>;
}; // namespace mcs::abnf::http