#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf
{

    /*
    month = %x4A.61.6E ; Jan
        / %x46.65.62 ; Feb
        / %x4D.61.72 ; Mar
        / %x41.70.72 ; Apr
        / %x4D.61.79 ; May
        / %x4A.75.6E ; Jun
        / %x4A.75.6C ; Jul
        / %x41.75.67 ; Aug
        / %x53.65.70 ; Sep
        / %x4F.63.74 ; Oct
        / %x4E.6F.76 ; Nov
        / %x44.65.63 ; Dec
    */
    constexpr abnf_result auto month(octet_param_in a, octet_param_in b,
                                     octet_param_in c) noexcept
    {
        return (a == 'J' && b == 'a' && c == 'n') || (a == 'F' && b == 'e' && c == 'b') ||
               (a == 'M' && b == 'a' && c == 'r') || (a == 'A' && b == 'p' && c == 'r') ||
               (a == 'M' && b == 'a' && c == 'y') || (a == 'J' && b == 'u' && c == 'n') ||
               (a == 'J' && b == 'u' && c == 'l') || (a == 'A' && b == 'u' && c == 'g') ||
               (a == 'S' && b == 'e' && c == 'p') || (a == 'O' && b == 'c' && c == 't') ||
               (a == 'N' && b == 'o' && c == 'v') || (a == 'D' && b == 'e' && c == 'c');
    }
    constexpr abnf_result auto month(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 3)
            return false;
        return month(sp[0], sp[1], sp[2]);
    }
} // namespace mcs::abnf