#pragma once

#include "./__obs_qtext.hpp"

namespace mcs::abnf::imf
{
    /**
     * qtext       =   %d33 /             ; Printable US-ASCII
                       %d35-91 /          ;  characters not including
                       %d93-126 /         ;  "\" or the quote character
                       obs-qtext
     *
     */
    using qtext = // NOLINTNEXTLINE
        alternative<CharSensitive<33>, Range<35, 91>, Range<93, 126>, obs_qtext>;

}; // namespace mcs::abnf::imf