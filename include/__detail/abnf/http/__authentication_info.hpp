#pragma once

#include "./__auth_param.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Authentication-Info = [ auth-param *( OWS "," OWS auth-param ) ]
    using Authentication_Info = optional<
        sequence<auth_param, zero_or_more<sequence<OWS, Char<','>, OWS, auth_param>>>>;
}; // namespace mcs::abnf::http