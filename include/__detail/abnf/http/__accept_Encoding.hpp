#pragma once

#include "./__codings.hpp"
#include "./__weight.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    /**
     * Accept-Encoding = [ ( codings [ weight ] ) *( OWS "," OWS ( codings [
                                                    weight ] ) ) ]
     *
     */
    using Accept_Encoding = optional<
        sequence<codings, optional<weight>,
                 zero_or_more<sequence<OWS, Char<','>, OWS, codings, optional<weight>>>>>;
}; // namespace mcs::abnf::http