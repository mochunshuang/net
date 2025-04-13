#pragma once

#include "./__dec_octet.hpp"
#include <array>

namespace mcs::abnf::uri
{
    // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
    constexpr bool IPv4address(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        constexpr auto k_min_size = 7; // 1.1.1.1
        if (k_size < k_min_size)
            return false;
        static_assert(not dec_octet(std::array<OCTET, 1>{'.'}));
        size_t idx1{};
        size_t idx2{};
        size_t idx3{};
        for (size_t i = 0, count = 0; i < k_size; ++i)
        {
            if (sp[i] == '.')
            {
                if (count == 0)
                {
                    idx1 = i;
                    ++count;
                    continue;
                }
                if (count == 1)
                {
                    idx2 = i;
                    ++count;
                    continue;
                }
                idx3 = i;
                break;
            }
        }
        // NOTE: idx3 + 1 == sp.size() then the last dec-octet is empty_span
        if (idx1 < idx2 && idx2 < idx3 && idx3 + 1 < sp.size())
            return dec_octet(sp.first(idx1)) &&
                   dec_octet(sp.subspan(idx1 + 1, idx2 - idx1 - 1)) &&
                   dec_octet(sp.subspan(idx2 + 1, idx3 - idx2 - 1)) &&
                   dec_octet(sp.subspan(idx3 + 1));
        return false;
    }
}; // namespace mcs::abnf::uri