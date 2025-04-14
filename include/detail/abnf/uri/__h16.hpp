#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::uri
{
    // h16           = 1*4HEXDIG
    constexpr CheckResult h16(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0 || k_size > 4)
            return Fail(0);

        for (size_t i{}; i < k_size; ++i)
        {
            if (not HEXDIG(sp[i]))
                return Fail(i);
        }
        return Success{k_size};
    }
}; // namespace mcs::abnf::uri