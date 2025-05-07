#pragma once

#include "./__protocol_name.hpp"
#include "./__protocol_version.hpp"

namespace mcs::abnf::http
{
    // received-protocol = [ protocol-name "/" ] protocol-version
    using received_protocol =
        sequence<optional<sequence<protocol_name, Char<'/'>>>, protocol_version>;
}; // namespace mcs::abnf::http