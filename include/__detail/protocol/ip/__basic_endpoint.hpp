#pragma once

#include "./__address.hpp"
#include "./__port_type.hpp"
#include "./__address_v4.hpp"
#include "./__address_v6.hpp"

namespace mcs::protocol::ip
{
    template <class InternetProtocol>
    struct basic_endpoint
    {
      public:
        // types:
        using protocol_type = InternetProtocol;

        // 21.13.1, constructors:
        // Postconditions: this->address() == ip::address() and port() == 0
        constexpr basic_endpoint() noexcept : port_{0} {}
        constexpr basic_endpoint(const protocol_type &proto, port_type port_num) noexcept
            : port_{port_num}
        {
            // Requires: proto == protocol_type::v4() || proto == protocol_type::v6()
            // Postconditions:
            // If proto == protocol_type::v6(), this->address() == ip::address_v6();
            // otherwise,this->address() == ip::address_v4()
            if (proto == protocol_type::v6())
                address_ = ip::address_v6();
            else
                address_ = ip::address_v4();
        }
        // Postconditions: this->address() == addr and port() == port_num.
        constexpr basic_endpoint(const ip::address &addr, port_type port_num) noexcept
            : address_{addr}, port_{port_num}
        {
        }
        constexpr basic_endpoint(const ip::address_v4 &addr, port_type port_num) noexcept
            : address_{addr}, port_{port_num}
        {
        }
        constexpr basic_endpoint(const ip::address_v6 &addr, port_type port_num) noexcept
            : address_{addr}, port_{port_num}
        {
        }
        // 21.13.2, members:
        constexpr protocol_type protocol() const noexcept
        {
            // Returns: protocol_type::v6() if the expression this->address().is_v6() is
            // true, otherwise protocol_type::v4().
            return this->address().is_v6() ? protocol_type::v6() : protocol_type::v4();
        }
        // Returns: The address associated with the endpoint.
        [[nodiscard]] constexpr ip::address address() const noexcept
        {
            return address_;
        }
        // Postconditions: this->address() == addr.
        void address(const ip::address &addr) noexcept
        {
            address_ = addr;
        }
        // Returns: The port number associated with the endpoint
        [[nodiscard]] constexpr port_type port() const noexcept
        {
            return port_;
        }
        // Postconditions: port() == port_num.
        void port(port_type port_num) noexcept
        {
            port_ = port_num;
        }

        // comparisons
        friend constexpr bool operator==(const basic_endpoint &,
                                         const basic_endpoint &) = default;
        friend constexpr auto operator<=>(const basic_endpoint &a,
                                          const basic_endpoint &b) noexcept = default;

      private:
        ip::address address_;
        port_type port_;
    };

}; // namespace mcs::protocol::ip