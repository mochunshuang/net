#pragma once

#include "./__challenge.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Proxy-Authenticate = [ challenge *( OWS "," OWS challenge ) ]
    using Proxy_Authenticate = optional<
        sequence<challenge, zero_or_more<sequence<OWS, Char<','>, OWS, challenge>>>>;

}; // namespace mcs::abnf::http