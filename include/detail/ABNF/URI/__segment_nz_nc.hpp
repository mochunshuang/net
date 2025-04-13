#pragma once

#include "./__reg_name.hpp"

namespace mcs::abnf::uri
{
    // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    // ;non-zero-length segment without any colon ":"
    constexpr CheckResult segment_nz_nc(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return Fail(0);

        // handle: 1*( unreserved / pct-encoded / sub-delims / "@" )
        // NOTE: reg-name = *( unreserved / pct-encoded / sub-delims )
        size_t index = 0;
        while (index < k_size)
        {
            const auto k_ret = reg_name(sp.subspan(index));
            if (k_ret)
            {
                index += k_ret->count;
                continue;
            }

            static_assert(not reg_name(std::array<OCTET, 1>{'@'}));
            index = index + k_ret.error().index();
            if (sp[index] == '@')
            {
                ++index;
                continue;
            }
            return Fail(index);
        }
        return Success{k_size};
    }
}; // namespace mcs::abnf::uri