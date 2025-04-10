#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::URI
{
    // pct-encoded   = "%" HEXDIG HEXDIG
    constexpr CheckResult pct_encoded(default_span_t sp) noexcept
    {
        if (sp.size() != 3)
            return std::unexpected(Info(0));

        if (sp[0] == '%') // "%41"='A'; %6A = j
        {
            if (not HEXDIG(sp[1]))
                return std::unexpected(Info(1));
            if (not HEXDIG(sp[2]))
                return std::unexpected(Info(2));
            return Success{3};
        }
        return std::unexpected(Info(0));
    }
}; // namespace mcs::ABNF::URI