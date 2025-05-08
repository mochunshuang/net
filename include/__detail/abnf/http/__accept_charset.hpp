#pragma once

#include "./__token.hpp"
#include "./__weight.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    /**
     * Accept-Charset = [ ( ( token / "*" ) [ weight ] ) *( OWS "," OWS ( (
                                                            token / "*" ) [ weight ] ) ) ]
     *
     */
    using Accept_Charset = optional<
        sequence<alternative<token, Char<'*'>>, optional<weight>,
                 zero_or_more<sequence<OWS, Char<','>, OWS, alternative<token, Char<'*'>>,
                                       optional<weight>>>>>;
}; // namespace mcs::abnf::http