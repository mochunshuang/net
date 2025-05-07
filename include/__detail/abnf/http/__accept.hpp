#pragma once

#include "./__media_range.hpp"
#include "./__weight.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    /**
     *Accept = [ ( media-range [ weight ] ) *( OWS "," OWS ( media-range [
                                                             weight ] ) ) ]
     *
     */
    using Accept_Language = optional<sequence<
        media_range, optional<weight>,
        zero_or_more<sequence<OWS, Char<','>, OWS, media_range, optional<weight>>>>>;
}; // namespace mcs::abnf::http