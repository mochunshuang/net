
#pragma once

#include "./__typefwd.hpp"
#include "../__socket_types.hpp"
#include <ostream>

#include "./__address.hpp"

namespace mcs::net::ip
{
    template <class InternetProtocol>
    class basic_endpoint
    {
      public:
        // types:
        using protocol_type = InternetProtocol;

        // 21.13.1, constructors:
        // Postconditions: this->address() == ip::address() and port() == 0
        constexpr basic_endpoint() noexcept = default;
        //  Requires: proto == protocol_type::v4() || proto == protocol_type::v6()
        constexpr basic_endpoint(const protocol_type &proto, port_type port_num) noexcept
        {
            if (proto == protocol_type::v6())
                this->address() == ip::address_v6();
            else
                this->address() == ip::address_v4();
            port_ = port_num;
        }
        // Postconditions: this->address() == addr and port() == port_num
        constexpr basic_endpoint(const ip::address &addr, port_type port_num) noexcept
            : data_{addr}, port_(port_num)
        {
        }
        // 21.13.2, members:
        constexpr protocol_type protocol() const noexcept
        {
            if (data_.is_v6())
                return protocol_type::v6();
            return protocol_type::v4();
        }
        // Returns: The address associated with the endpoint
        [[nodiscard]] constexpr ip::address address() const noexcept
        {
            return data_;
        }
        // Postconditions: this->address() == addr
        void address(const ip::address &addr) noexcept
        {
            data_ = addr;
        }
        [[nodiscard]] constexpr port_type port() const noexcept
        {
            return port_;
        }
        // Postconditions: port() == port_num
        void port(port_type port_num) noexcept
        {
            port_ = port_num;
        }

        // Returns: addressof(data_)
        void *data() noexcept
        {
            return std::addressof(data_);
        }
        //  Returns: addressof(data_)
        [[nodiscard]] const void *data() const noexcept
        {
            return std::addressof(data_);
        }
        // TODO(mcs):
        // Returns: sizeof(sockaddr_in6) if protocol().family() == AF_INET6, otherwise
        // sizeof(sockaddr_in).
        [[nodiscard]] constexpr size_t size() const noexcept;
        void resize(size_t s);
        [[nodiscard]] constexpr size_t capacity() const noexcept;

        // 21.13.3, basic_endpoint comparisons:
        friend constexpr bool operator==(const basic_endpoint &,
                                         const basic_endpoint &) = default;
        friend constexpr auto operator<=>(const basic_endpoint &a,
                                          const basic_endpoint &b) noexcept
            -> std::strong_ordering = default;

      private:
        // union {
        //     sockaddr_in4_type v4;
        //     sockaddr_in6_type v6;
        // } data_; // exposition only
        ip::address data_;
        protocol_type port_{};
    };

    // 21.13.4, basic_endpoint I/O:
    template <class CharT, class Traits, class InternetProtocol>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os,
        const basic_endpoint<InternetProtocol> &ep);

}; // namespace mcs::net::ip