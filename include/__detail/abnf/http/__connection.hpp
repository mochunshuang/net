#pragma once

#include "./__connection_option.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Connection = [ connection-option *( OWS "," OWS connection-option ) ]
    using Connection = optional<
        sequence<connection_option,
                 zero_or_more<sequence<OWS, Char<','>, OWS, connection_option>>>>;
}; // namespace mcs::abnf::http