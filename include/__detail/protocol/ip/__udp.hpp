#pragma once

#include "./__basic_endpoint.hpp"
#include "./__detail/__sysytem_dependent.hpp"
#include "./__basic_socket.hpp"

namespace mcs::protocol::ip
{
    struct udp
    {
        /// The type of a UDP endpoint.
        using endpoint = basic_endpoint<udp>;
        using socket = basic_socket<udp>;

        /// Construct to represent the IPv4 UDP protocol.
        static udp v4() noexcept
        {
            return udp(OS_DEF_AF_INET);
        }

        /// Construct to represent the IPv6 UDP protocol.
        static udp v6() noexcept
        {
            return udp(OS_DEF_AF_INET6);
        }

        /// Obtain an identifier for the type of the protocol.
        static int type() noexcept
        {
            return OS_DEF_SOCK_DGRAM;
        }

        /// Obtain an identifier for the protocol.
        static int protocol() noexcept
        {
            return OS_DEF_IPPROTO_UDP;
        }

        /// Obtain an identifier for the protocol family.
        [[nodiscard]] constexpr int family() const noexcept
        {
            return family_;
        }

        friend constexpr bool operator==(const udp &a, const udp &b) noexcept = default;
        friend constexpr bool operator!=(const udp &a, const udp &b) noexcept = default;

      private:
        // Construct with a specific family.
        explicit udp(int protocol_family) noexcept : family_(protocol_family) {}

        int family_;
    };
}; // namespace mcs::protocol::ip