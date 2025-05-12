#pragma once

#include "./__auth_param.hpp"
#include "./__auth_scheme.hpp"
#include "./__ows.hpp"
#include "./__token68.hpp"

namespace mcs::abnf::http
{
    /**
     * challenge = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS ","
                   OWS auth-param ) ] ) ]
     */
    using challenge = sequence<
        auth_scheme,
        optional<sequence<
            one_or_more<SP>,
            alternative< // NOTE: change by "auth-scheme = token" that like token68
                optional<sequence<
                    auth_param, zero_or_more<sequence<OWS, Char<','>, OWS, auth_param>>>>,
                token68>>>>;
}; // namespace mcs::abnf::http