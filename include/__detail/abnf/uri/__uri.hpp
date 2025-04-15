#pragma once

#include "./__scheme.hpp"
#include "./__hier_part.hpp"
#include "./__query.hpp"
#include "./__fragment.hpp"

namespace mcs::abnf::uri
{
    // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
    constexpr auto URI(span_param_in sp) noexcept
    {
        using builder = result_builder<result<4>>;
        const auto k_size = sp.size();
        if (k_size < 2)
            return builder::fail(0);
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
            return builder::fail(0);

        const auto k_sche = sp.first(idx_0);
        if (not scheme(k_sche))
            return builder::fail(0);

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
            if (hier_part(k_hier))
                return builder::success(span{.start = 0, .count = k_sche.size()},
                                        span{.start = idx_0 + 1, .count = k_hier.size()},
                                        invalid_span, invalid_span);
            return builder::fail(0);
        }
        if (idx_1 < k_size && idx_2 == k_size) // scheme ":" hier-part "?" query
        {
            const auto k_hier = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
            const auto k_query = sp.subspan(idx_1 + 1);
            if (hier_part(k_hier) && query(k_query))
                return builder::success(span{.start = 0, .count = k_sche.size()},
                                        span{.start = idx_0 + 1, .count = k_hier.size()},
                                        span{.start = idx_1 + 1, .count = k_query.size()},
                                        invalid_span);
            return builder::fail(0);
        }
        if (idx_1 == k_size && idx_2 < k_size) // scheme ":" hier-part "#" fragment
        {
            const auto k_hier = sp.subspan(idx_0 + 1, idx_2 - idx_0 - 1);
            const auto k_fragment = sp.subspan(idx_2 + 1);
            if (hier_part(k_hier) && fragment(k_fragment))
                return builder::success(
                    span{.start = 0, .count = k_sche.size()},
                    span{.start = idx_0 + 1, .count = k_hier.size()}, invalid_span,
                    span{.start = idx_2 + 1, .count = k_fragment.size()});
            return builder::fail(0);
        }
        // scheme ":" hier-part "?" query  "#" fragment
        const auto k_hier = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        const auto k_query = sp.subspan(idx_1 + 1, idx_2 - idx_1 - 1);
        const auto k_fragment = sp.subspan(idx_2 + 1);
        if (hier_part(k_hier) && query(k_query) && fragment(k_fragment))
            return builder::success(span{.start = 0, .count = k_sche.size()},
                                    span{.start = idx_0 + 1, .count = k_hier.size()},
                                    span{.start = idx_1 + 1, .count = k_query.size()},
                                    span{.start = idx_2 + 1, .count = k_fragment.size()});
        return builder::fail(0);
    }
}; // namespace mcs::abnf::uri