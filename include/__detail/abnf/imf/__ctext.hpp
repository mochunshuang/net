#pragma once

#include "../__core_types.hpp"
#include "./__obs_ctext.hpp"

namespace mcs::abnf::imf
{
    /*
    ctext           =   %d33-39 /          ; Printable US-ASCII
                        %d42-91 /          ;  characters not including
                        %d93-126 /         ;  "(", ")", or "\"
                        obs-ctext
    */
    constexpr abnf_result auto ctext(octet_param_in c) noexcept
    {
        // NOLINTNEXTLINE
        return (c >= 33 && c <= 39) || (c >= 42 && c <= 91) || (c >= 93 && c <= 126) ||
               obs_ctext(c);
    }
} // namespace mcs::abnf::imf