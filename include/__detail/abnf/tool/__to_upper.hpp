#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr detail::octet to_upper(detail::octet ch) noexcept
    {
        constexpr std::uint8_t k_distance = 32;
        return (ch >= 'a' && ch <= 'z') ? ch - k_distance : ch;
    };
}; // namespace mcs::abnf::tool
