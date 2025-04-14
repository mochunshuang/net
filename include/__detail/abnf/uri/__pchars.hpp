#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{

    // pchars         = *pchar
    constexpr CheckResult pchars(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        size_t index = 0;
        while (index < k_size)
        {
            const auto &c = sp[index];

            // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
            static_assert(not pchar('%'));
            static_assert(not unreserved('%'));
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
                if (pchar(c))
                {
                    index++;
                    continue;
                }
            }
            return Fail(index);
        }
        return Success{k_size};
    }
}; // namespace mcs::abnf::uri