#pragma once

#include "../tool/__safe_subspan.hpp"
#include <algorithm>

namespace mcs::abnf::uri
{
    // scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    // https://www.rfc-editor.org/rfc/rfc3986.html#appendix-A:~:text=scheme%20%20%20%20%20%20%20%20%3D%20ALPHA%20*(%20ALPHA%20/%20DIGIT%20/%20%22%2B%22%20/%20%22%2D%22%20/%20%22.%22%20)
    constexpr bool scheme(default_span_t range) noexcept
    {
        if (range.empty())
            return false;

        if (not ALPHA(range[0]))
            return false;

        const auto k_remaining = tool::safe_subspan(range, 1);
        return std::ranges::all_of(k_remaining, [](octet_t v) noexcept -> bool {
            return ALPHA(v) || DIGIT(v) || v == '+' || v == '-' || v == '.';
        });
    }

}; // namespace mcs::abnf::uri