#pragma once

#include "./__pchar.hpp"

namespace mcs::ABNF::URI
{

    // pchars         = *pchar
    constexpr CheckResult pchars(default_span_t sp) noexcept
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
            return std::unexpected{Info(index)};
        }
        return Success{k_size};
    }
}; // namespace mcs::ABNF::URI