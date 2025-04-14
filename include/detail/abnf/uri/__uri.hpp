#pragma once

#include "./__scheme.hpp"
#include "./__hier_part.hpp"
#include "./__query.hpp"
#include "./__fragment.hpp"

namespace mcs::abnf::uri
{
    // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    constexpr bool URI(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 2)
            return false;
        size_t idx_0 = k_size; // index: ':'
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

        size_t idx_1 = k_size; // index: '?'
        for (size_t i = idx_0 + 1; i < k_size; ++i)
        {
            if (sp[i] == '?')
            {
                idx_1 = i;
                break;
            }
        }
        size_t idx_2 = k_size; // index: '#'
        for (size_t left = idx_1 < k_size ? idx_1 + 1 : idx_0 + 1, right = k_size;
             right-- > left;)
        {
            if (sp[right] == '#')
            {
                idx_2 = right;
                break;
            }
        }

        if (idx_1 == k_size && idx_2 == k_size) // scheme ":" hier-part only
        {
            const auto k_hier = sp.subspan(idx_0 + 1);
            return hier_part(k_hier);
        }
        if (idx_1 < k_size && idx_2 == k_size) // scheme ":" hier-part "?" query
        {
            const auto k_hier = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
            const auto k_query = sp.subspan(idx_1 + 1);
            return hier_part(k_hier) && query(k_query);
        }
        if (idx_1 == k_size && idx_2 < k_size) // scheme ":" hier-part "#" fragment
        {
            const auto k_hier = sp.subspan(idx_0 + 1, idx_2 - idx_0 - 1);
            const auto k_fragment = sp.subspan(idx_2 + 1);
            return hier_part(k_hier) && fragment(k_fragment);
        }
        // scheme ":" hier-part "?" query  "#" fragment
        const auto k_hier = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        const auto k_query = sp.subspan(idx_1 + 1, idx_2 - idx_1 - 1);
        const auto k_fragment = sp.subspan(idx_2 + 1);
        return hier_part(k_hier) && query(k_query) && fragment(k_fragment);
    }
}; // namespace mcs::abnf::uri