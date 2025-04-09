#pragma once

#include "../__core_rules.hpp"

#include "./__pchar.hpp"

namespace mcs::ABNF::URI
{
    // query         = *( pchar / "/" / "?" )
    constexpr bool query(default_span_t sp) noexcept
    {
        size_t index = 0;
        while (index < sp.size())
        {
            bool matched = false;

            if (index + 3 <= sp.size())
            {
                const auto k_sub = sp.subspan(index, 3);
                if (pchar(k_sub))
                {
                    index += 3;
                    matched = true;
                }
            }

            if (!matched && index < sp.size())
            {
                const auto &c = sp[index];
                if (pchar(c) || c == '/' || c == '?')
                {
                    index++;
                    matched = true;
                }
            }

            if (!matched)
                return false;
        }
        return true; // sp.empty() || pass
    }
}; // namespace mcs::ABNF::URI