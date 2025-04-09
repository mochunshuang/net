#pragma once
#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::ABNF::URI
{
    // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    constexpr bool pchar(octet_t v) noexcept
    {
        return unreserved(v) || sub_delims(v) || (v == ':' || v == '@');
    }
    constexpr bool pchar(default_span_t sp) noexcept
    {
        // NOTE: ABNF的顺序，一定要严格遵守
        // if (sp.size() == 1) //NOTE: 移动到到 pchar 是等价的
        //     return unreserved(sp[0]);
        if (sp.size() == 3)
            return pct_encoded(sp);
        if (sp.size() == 1)
            return pchar(sp[0]);
        return false;
    }
}; // namespace mcs::ABNF::URI