#pragma once

#include "./__method.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Allow = [ method *( OWS "," OWS method ) ]
    using Allow =
        optional<sequence<method, zero_or_more<sequence<OWS, Char<','>, OWS, method>>>>;

}; // namespace mcs::abnf::http