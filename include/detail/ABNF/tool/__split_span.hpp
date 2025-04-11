#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::tool
{
    constexpr auto split_span_first(default_span_t sp, octet_t c) noexcept
    {
        for (size_t i = 0; i < sp.size(); ++i)
        {
            if (sp[i] == c)
                return std::pair{sp.first(i), sp.subspan(i + 1)};
        }
        return std::pair{sp, empty_span}; // not find c in sp
    }
    constexpr auto split_span_last(default_span_t sp, octet_t c) noexcept
    {
        for (size_t i = sp.size(); i-- > 0;)
        {
            if (sp[i] == c)
                return std::pair{sp.first(i), sp.subspan(i + 1)};
        }
        return std::pair{empty_span, sp}; // not find c in sp
    }
}; // namespace mcs::ABNF::tool