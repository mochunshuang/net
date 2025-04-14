#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{
    // fragment      = *( pchar / "/" / "?" )
    constexpr auto fragment(span_param_in sp) noexcept -> abnf_result auto
    {
        const auto k_size = sp.size();
        size_t index = 0;
        while (index < k_size)
        {
            // pchar
            const auto &c = sp[index];
            if (c == '%')
            {
                if (index + 2 < k_size) // check: if match "%41"
                {
                    if (pchar(c, sp[index + 1], sp[index + 2]))
                    {
                        index += 3;
                        continue;
                    }
                }
            }
            else
            {
                static_assert(not pchar('%'));
                if (pchar(c))
                {
                    index++;
                    continue;
                }
            }

            if (c == '/' || c == '?')
            {
                ++index;
                continue;
            }
            return false;
        }
        return true;
    }
}; // namespace mcs::abnf::uri