#pragma once

#include "./__pchar.hpp"

namespace mcs::ABNF::URI
{
    // segment-nz    = 1*pchar
    constexpr bool segment_nz(default_span_t sp) noexcept
    {
        if (sp.empty())
            return false;

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
                if (pchar(c))
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