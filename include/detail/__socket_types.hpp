#pragma once

#include <array>
#include <compare>
#include <cstdint>

#include "./__core_types.hpp"

namespace mcs::net
{

    using in4_addr_t = std::uint_least32_t;
    using in6_addr_t = std::array<unsigned char, 16>; // NOLINT
    struct in4_addr_type
    {
        in4_addr_t s_addr;
        friend constexpr bool operator==(const in4_addr_type &,
                                         const in4_addr_type &) = default;
        friend constexpr auto operator<=>(const in4_addr_type &a,
                                          const in4_addr_type &b) noexcept
            -> std::strong_ordering = default;
    };
    struct in6_addr_type
    {
        in6_addr_t s6_addr;
        friend constexpr bool operator==(const in6_addr_type &,
                                         const in6_addr_type &) = default;
        friend constexpr auto operator<=>(const in6_addr_type &a,
                                          const in6_addr_type &b) noexcept
            -> std::strong_ordering = default;
    };

    struct sockaddr_in4_type
    {
        int sin_family;
        in4_addr_type sin_addr;
        in4_addr_t sin_port;
    };
    struct sockaddr_in6_type
    {
        int sin6_family;
        in6_addr_type sin6_addr;
        port_type sin6_port;
        scope_id_type sin6_flowinfo;
        scope_id_type sin6_scope_id;
    };

}; // namespace mcs::net