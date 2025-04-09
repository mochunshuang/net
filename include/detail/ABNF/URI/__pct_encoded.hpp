#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::URI
{
    // pct-encoded   = "%" HEXDIG HEXDIG
    constexpr bool pct_encoded(default_span_t sp) noexcept
    {
        if (sp.size() != 3)
            return false; // 输入"%41"（即"A"的编码），%6A = j
        return sp[0] == '%' && HEXDIG(sp[1]) && HEXDIG(sp[2]);
    }
}; // namespace mcs::ABNF::URI