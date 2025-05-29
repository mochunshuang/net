#pragma once

namespace mcs::taps
{
    /**
     * @brief The following API calls can be used to configure a Preconnection before
     * calling Rendezvous:
        RemoteSpecifier.WithMulticastGroupIP(GroupAddress)
        RemoteSpecifier.WithPort(PortNumber)
        RemoteSpecifier.WithHopLimit(HopLimit)
        LocalSpecifier.WithAnySourceMulticastGroupIP(GroupAddress)
        LocalSpecifier.WithPort(PortNumber)
        LocalSpecifier.WithHopLimit(HopLimit)
     */
    struct local_specifier
    {
        void withSingleSourceMulticastGroupIp();
        void withAnySourceMulticastGroupIp();
        void withPort();
    };
}; // namespace mcs::taps