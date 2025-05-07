#pragma once

#include "./__language_range.hpp"
#include "./__weight.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    /**
     * Accept-Language = [ ( language-range [ weight ] ) *( OWS "," OWS (
                             language-range [ weight ] ) ) ]
     *
     */
    using Accept_Language = optional<sequence<
        language_range, optional<weight>,
        zero_or_more<sequence<OWS, Char<','>, OWS, language_range, optional<weight>>>>>;
}; // namespace mcs::abnf::http