#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::http
{
    /*
    day-name = %x4D.6F.6E ; Mon
            / %x54.75.65 ; Tue
            / %x57.65.64 ; Wed
            / %x54.68.75 ; Thu
            / %x46.72.69 ; Fri
            / %x53.61.74 ; Sat
            / %x53.75.6E ; Sun
    */
    constexpr abnf_result auto day_name(octet_param_in a, octet_param_in b,
                                        octet_param_in c) noexcept
    {
        return (a == 'M' && b == 'o' && c == 'n') || (a == 'T' && b == 'u' && c == 'e') ||
               (a == 'W' && b == 'e' && c == 'd') || (a == 'T' && b == 'h' && c == 'u') ||
               (a == 'F' && b == 'r' && c == 'i') || (a == 'S' && b == 'a' && c == 't') ||
               (a == 'S' && b == 'u' && c == 'n');
    }

    constexpr abnf_result auto day_name(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 3)
            return false;
        return day_name(sp[0], sp[1], sp[2]);
    }

}; // namespace mcs::abnf::http