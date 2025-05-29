#pragma once

#include <array>
#include <compare>
#include <cstdint>

namespace mcs::protocol::ip::__detail
{
    struct address_v4_store
    {
        using type = std::uint_least32_t;
        type value;

        friend constexpr auto operator<=>(const address_v4_store &a,
                                          const address_v4_store &b) noexcept
            -> std::strong_ordering = default;
    };

    struct address_v6_store
    {
        using type = std::array<unsigned char, 16>; // NOLINT
        type value;

        friend constexpr auto operator<=>(const address_v6_store &a,
                                          const address_v6_store &b) noexcept
            -> std::strong_ordering = default;
    };

}; // namespace mcs::protocol::ip::__detail