#pragma once

#include "./__protocol_name.hpp"
#include "./__protocol_version.hpp"

namespace mcs::abnf::http
{
    // protocol = protocol-name [ "/" protocol-version ]
    using protocol = sequence<protocol_name,
                              optional<sequence<optional<Char<'/'>>, protocol_version>>>;
}; // namespace mcs::abnf::http