#pragma once

#include "../__core_rules.hpp"
#include <algorithm>

namespace mcs::ABNF::URI
{
    // port          = *DIGIT
    constexpr bool port(default_span_t sp) noexcept
    {
        if (sp.size() == 0)
            return true;
        return std::ranges::all_of(sp, [](const auto &v) { return DIGIT(v); });
    }
}; // namespace mcs::ABNF::URI