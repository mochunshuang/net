#pragma once

#include "./__obs_ctext.hpp"

namespace mcs::abnf::imf
{
    /*
    ctext           =   %d33-39 /          ; Printable US-ASCII
                        %d42-91 /          ;  characters not including
                        %d93-126 /         ;  "(", ")", or "\"
                        obs-ctext
    */
    using ctext =
        alternative<Range<33, 39>, Range<42, 91>, Range<93, 126>, obs_ctext>; // NOLINT
}; // namespace mcs::abnf::imf