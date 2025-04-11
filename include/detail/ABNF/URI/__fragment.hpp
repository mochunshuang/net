#pragma once

#include "./__pchars.hpp"
#include "__pchar.hpp"

namespace mcs::ABNF::URI
{
    // fragment      = *( pchar / "/" / "?" )
    constexpr CheckResult fragment(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        size_t index = 0;
        while (index < k_size)
        {
            const auto k_check_span = sp.subspan(index);
            const auto k_ret = pchars(k_check_span);
            if (k_ret) // NOTE: first check pchar anyway
            {
                index += k_ret->count;
                continue;
            }

            // NOTE: error: ...%ce... => ret.error().index() -> %
            // NOTE: error: ...{... => ret.error().index() -> %
            // NOTE: error: .../... => ret.error().index() -> /
            static_assert(not pchar('{'));
            static_assert(not pchar('/') && not pchar('?'));

            index = k_ret.error().index();
            const auto &c = sp[index];
            if (c == '/' || c == '?')
            {
                ++index;
                continue;
            }
            return Fail(index);
        }
        return Success{k_size};
    }
}; // namespace mcs::ABNF::URI