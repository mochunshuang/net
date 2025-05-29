#pragma once

#include "./__basic_endpoint.hpp"
#include "./__detail/__sysytem_dependent.hpp"
#include "./__basic_socket.hpp"

namespace mcs::protocol::ip
{
    struct tcp
    {
      public:
        /// The type of a TCP endpoint.
        using endpoint = basic_endpoint<tcp>;
        // using resolver = basic_resolver<tcp>;
        using socket = basic_socket<tcp>;
        // using acceptor = basic_socket_acceptor<tcp>;
        // using iostream = basic_socket_iostream<tcp>;
        // class no_delay;

        /// Construct to represent the IPv4 TCP protocol.
        static tcp v4() noexcept
        {
            return tcp(OS_DEF_AF_INET);
        }

        /// Construct to represent the IPv6 TCP protocol.
        static tcp v6() noexcept
        {
            return tcp(OS_DEF_AF_INET6);
        }

        /// Obtain an identifier for the type of the protocol.
        static int type() noexcept
        {
            return OS_DEF_SOCK_STREAM;
        }

        /// Obtain an identifier for the protocol.
        static int protocol() noexcept
        {
            return OS_DEF_IPPROTO_TCP;
        }

        /// Obtain an identifier for the protocol family.
        [[nodiscard]] constexpr int family() const noexcept
        {
            return family_;
        }

        // 21.19.1, tcp comparisons:
        friend constexpr bool operator==(const tcp &a, const tcp &b) noexcept = default;
        friend constexpr bool operator!=(const tcp &a, const tcp &b) noexcept = default;

      private:
        // Construct with a specific family.
        constexpr explicit tcp(int protocol_family) noexcept : family_(protocol_family) {}
        int family_;
    };
}; // namespace mcs::protocol::ip