#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::tool
{
    constexpr auto split_span_first(span_param_in sp, octet_param_in c) noexcept
    {
        // NOTE: first(i): i < k_size is ok
        // NOTE: subspan(i+1): i+1 <=k_size is ok
        const auto k_size = sp.size();
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == c)
                return std::pair{sp.first(i), sp.subspan(i + 1)};
        }
        return std::pair{sp, empty_span_param}; // not find c in sp
    }
    constexpr auto split_span_last(span_param_in sp, octet_param_in c) noexcept
    {
        const auto k_size = sp.size();
        for (size_t i = k_size; i-- > 0;)
        {
            if (sp[i] == c)
                return std::pair{sp.first(i), sp.subspan(i + 1)};
        }
        return std::pair{empty_span_param, sp}; // not find c in sp
    }
}; // namespace mcs::abnf::tool