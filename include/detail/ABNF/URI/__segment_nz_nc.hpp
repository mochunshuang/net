#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::ABNF::URI
{
    // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    // ;non-zero-length segment without any colon ":"
    constexpr bool segment_nz_nc(default_span_t sp) noexcept
    {
        if (sp.empty())
            return false;

        size_t index = 0;
        while (index < sp.size())
        {
            bool matched = false;

            if (unreserved(sp[index])) // 1、 unreserved
            {
                ++index;
                matched = true;
            }

            if (!matched && index + 3 <= sp.size()) // 2、 pct-encoded
            {
                const auto k_sub = sp.subspan(index, 3);
                if (pct_encoded(k_sub))
                {
                    index += 3;
                    matched = true;
                }
            }

            if (!matched && index < sp.size()) // 3、sub-delims / "@"
            {
                const auto &c = sp[index];
                if (sub_delims(c) || c == '@')
                {
                    ++index;
                    matched = true;
                }
            }

            if (!matched)
                return false;
        }
        return true;
    }
}; // namespace mcs::ABNF::URI