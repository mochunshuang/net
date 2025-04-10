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
            auto check_span = sp.subspan(index);
            auto ret = pchars(check_span);
            if (ret) // NOTE: first check pchar anyway
            {
                index += ret->count;
                continue;
            }

            // NOTE: error: ...%ce... => ret.error().index() -> %
            // NOTE: error: ...{... => ret.error().index() -> %
            // NOTE: error: .../... => ret.error().index() -> /
            static_assert(not pchar('{'));
            static_assert(not pchar('/') && not pchar('?'));

            index = ret.error().index();
            const auto &c = sp[index];
            if (c == '/' || c == '?')
            {
                ++index;
                continue;
            }
            return std::unexpected{Info(index)};
        }
        return Success{k_size};
    }
}; // namespace mcs::ABNF::URI