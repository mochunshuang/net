#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace mcs::taps
{
    /**
     * @brief
     RemoteSpecifier.WithHostName("example.com")

     */
    struct remote_specifier
    {
        using HostNameType = std::string; // HostName (string):
        using PortType = std::uint16_t;   // Port (a 16-bit unsigned Integer):

        void withHostName(HostNameType hostName) noexcept;
        void withPort(PortType port) noexcept;
        void withService(std::string_view name_service) noexcept;
        void withIpAddress() noexcept; // TODO(mcs): IP address (an IPv4 or IPv6 address
                                       // type;
        void withInterface(std::string_view name_interface) noexcept;
        void withMulticastGroupIp() noexcept; // TODO(mcs): IP address
        void withHopLimit() noexcept;
    };
}; // namespace mcs::taps