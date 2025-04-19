#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::http
{
    // GMT = %x47.4D.54 ; GMT
    constexpr abnf_result auto GMT(octet_param_in a, octet_param_in b,
                                   octet_param_in c) noexcept
    {
        return (a == 'G' && b == 'M' && c == 'T');
    }

    constexpr abnf_result auto GMT(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 3)
            return false;
        return GMT(sp[0], sp[1], sp[2]);
    }

}; // namespace mcs::abnf::http