#pragma once

#include "./__auth_scheme.hpp"
#include "./__token68.hpp"
#include "./__auth_param.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    /**
     * credentials = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS ","
                                 OWS auth-param ) ] ) ]
     *
     */
    using credentials = sequence<
        auth_scheme,
        optional<sequence<
            one_or_more<SP>,
            alternative<token68, optional<sequence<auth_param, zero_or_more<sequence<
                                                                   OWS, Char<','>, OWS,
                                                                   auth_param>>>>>>>>;
}; // namespace mcs::abnf::http