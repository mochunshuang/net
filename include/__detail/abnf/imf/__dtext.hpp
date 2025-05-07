#pragma once

#include "./__obs_dtext.hpp"

namespace mcs::abnf::imf
{
    /**
     * dtext       =   %d33-90 /          ; Printable US-ASCII
                       %d94-126 /         ;  characters not including
                       obs-dtext          ;  "[", "]", or "\"
     *
     */
    using dtext = alternative<Range<33, 90>, Range<94, 126>, obs_dtext>; // NOLINT
}; // namespace mcs::abnf::imf