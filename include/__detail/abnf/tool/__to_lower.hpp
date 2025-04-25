#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr detail::octet to_lower(detail::octet ch) noexcept
    {
        constexpr std::uint8_t k_distance = 32;
        return (ch >= 'A' && ch <= 'Z') ? ch + k_distance : ch;
    };
}; // namespace mcs::abnf::tool