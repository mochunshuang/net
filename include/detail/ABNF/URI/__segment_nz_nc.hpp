#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::ABNF::URI
{
    // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    // ;non-zero-length segment without any colon ":"
    constexpr CheckResult segment_nz_nc(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return std::unexpected{Info{0}};

        size_t index = 0;
        while (index < k_size)
        {
            const auto &c = sp[index];
            if (unreserved(c)) // 1、unreserved
            {
                ++index;
                continue;
            }

            static_assert(not sub_delims('%'));
            if (c == '%') // 2、pct-encoded   = "%" HEXDIG HEXDIG
            {
                if (index + 2 < k_size)
                {
                    if (pct_encoded(c, sp[index + 1], sp[index + 2]))
                    {
                        index += 3;
                        continue;
                    }
                }
            }
            else // 3、sub-delims / "@"
            {
                if (sub_delims(c) || c == '@')
                {
                    ++index;
                    continue;
                }
            }
            return std::unexpected{Info{index}};
        }
        return Success{k_size};
    }
}; // namespace mcs::ABNF::URI