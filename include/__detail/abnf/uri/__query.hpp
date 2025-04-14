#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{
    // query         = *( pchar / "/" / "?" )
    constexpr CheckResult query(span_param_in sp) noexcept
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
            return Fail(index);
        }
        return Success{k_size};
    }
}; // namespace mcs::abnf::uri