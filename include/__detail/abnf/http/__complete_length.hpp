#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // complete-length = 1*DIGIT
    constexpr abnf_result auto complete_length(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 1)
            return false;
        for (std::size_t i{0}; i < k_size; ++i)
        {
            if (not DIGIT(sp[i]))
                return false;
        }
        return true;
    }
}; // namespace mcs::abnf::http