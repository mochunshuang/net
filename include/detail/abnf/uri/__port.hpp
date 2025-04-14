#pragma once

#include "../__core_rules.hpp"
#include <algorithm>

namespace mcs::abnf::uri
{
    // port          = *DIGIT
    constexpr bool port(span_param_in sp) noexcept
    {
        if (sp.size() == 0)
            return true;
        return std::ranges::all_of(sp, [](const auto &v) { return DIGIT(v); });
    }
}; // namespace mcs::abnf::uri