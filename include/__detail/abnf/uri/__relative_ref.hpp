#pragma once

#include "./__relative_part.hpp"
#include "./__fragment.hpp"
#include "./__pchar.hpp"
#include "./__query.hpp"
#include <array>

namespace mcs::abnf::uri
{
    // relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    constexpr bool relative_ref(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        // NOTE: no need check k_size > 0
        static_assert(relative_part(empty_span_param));

        size_t idx_0 = k_size;
        size_t idx_1 = k_size;
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == '?')
            {
                idx_0 = i;
                break;
            }
        }
        for (size_t left = (idx_0 < k_size ? idx_0 + 1 : 0), right = k_size;
             right-- > left;)
        {
            if (sp[right] == '#')
            {
                idx_1 = right;
                break;
            }
        }

        // query         = *( pchar / "/" / "?" )
        // fragment      = *( pchar / "/" / "?" )
        static_assert(not pchar('#'));
        static_assert(not pchar('?'));
        static_assert(not fragment(std::array<OCTET, 1>{'#'}));

        if (idx_0 == k_size && idx_1 == k_size) // only relative_part
            return relative_part(sp);
        if (idx_0 < k_size && idx_1 == k_size) // relative_part and query
        {
            const auto k_r = sp.first(idx_0);
            const auto k_q = sp.subspan(idx_0 + 1);
            return relative_part(k_r) && query(k_q);
        }
        if (idx_0 == k_size && idx_1 < k_size) // relative_part and fragment
        {
            const auto k_r = sp.first(idx_1);
            const auto k_f = sp.subspan(idx_1 + 1);
            return relative_part(k_r) && fragment(k_f);
        }
        const auto k_r = sp.first(idx_0);
        const auto k_q = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        const auto k_f = sp.subspan(idx_1 + 1);
        return relative_part(k_r) && query(k_q) && fragment(k_f);
    }
}; // namespace mcs::abnf::uri