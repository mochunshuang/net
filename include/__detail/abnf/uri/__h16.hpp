#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::uri
{
    // h16           = 1*4HEXDIG
    constexpr auto h16(span_param_in sp) noexcept -> abnf_result auto
    {
        const auto k_size = sp.size();
        if (k_size == 0 || k_size > 4)
            return false;

        for (size_t i{}; i < k_size; ++i)
        {
            if (not HEXDIG(sp[i]))
                return false;
        }
        return true;
    }
}; // namespace mcs::abnf::uri