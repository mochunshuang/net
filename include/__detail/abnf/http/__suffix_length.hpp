#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // suffix-length = 1*DIGIT
    constexpr abnf_result auto suffix_length(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);
        for (std::size_t i{0}; i < k_size; ++i)
        {
            if (not DIGIT(sp[i]))
                return simple_result::fail(i);
        }
        return simple_result::success();
    }
}; // namespace mcs::abnf::http