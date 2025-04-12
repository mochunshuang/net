#pragma once

#include "./__scheme.hpp"
#include "./__hier_part.hpp"
#include "./__query.hpp"

namespace mcs::ABNF::URI
{
    // absolute-URI  = scheme ":" hier-part [ "?" query ]
    constexpr bool absolute_uri(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 2)
            return false;

        size_t idx_0 = k_size;
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == ':')
            {
                idx_0 = i;
                break;
            }
        }
        if (idx_0 == k_size)
            return false;
        const auto k_sche = sp.first(idx_0);
        if (not scheme(k_sche))
            return false;

        size_t idx_1 = k_size;
        for (size_t i = idx_0 + 1; i < k_size; ++i)
        {
            if (sp[i] == '?')
            {
                idx_1 = i;
                break;
            }
        }
        if (idx_1 == k_size)
        {
            const auto k_hier = sp.subspan(idx_0 + 1);
            return hier_part(k_hier);
        }
        const auto k_hier = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        const auto k_query = sp.subspan(idx_1 + 1);
        return hier_part(k_hier) && query(k_query);
    }
}; // namespace mcs::ABNF::URI