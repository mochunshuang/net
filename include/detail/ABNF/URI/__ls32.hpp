#pragma once

#include "./__h16.hpp"
#include "./__ipv4address.hpp"
#include <array>
#include <cstddef>

namespace mcs::abnf::uri
{
    // ls32          = ( h16 ":" h16 ) / IPv4address
    constexpr bool ls32(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return false;

        static_assert(not IPv4address(std::array<OCTET, 1>{':'}));
        static_assert(not h16(std::array<OCTET, 1>{':'}));
        static_assert(not h16(empty_span)); // NOTE: ":"xxx is error case

        if (const auto k_first = std::ranges::find(sp, ':');
            k_first != sp.end()) // find ':' in sp
        {
            size_t idx1 = std::distance(sp.begin(), k_first);
            if (idx1 == 0 || idx1 + 1 == k_size) // when sp like ":"xxx or  xxx":"
                return false;
            return h16(sp.first(idx1)) && h16(sp.subspan(idx1 + 1));
        }
        return IPv4address(sp);
    }
}; // namespace mcs::abnf::uri