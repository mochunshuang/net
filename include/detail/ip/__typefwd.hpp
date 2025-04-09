#pragma once

namespace mcs::net::ip
{
    class address;
    class address_v4;
    class address_v6;
    template <class Address>
    class basic_address_iterator;
    using address_v4_iterator = basic_address_iterator<address_v4>;
    using address_v6_iterator = basic_address_iterator<address_v6>;

    template <class Address>
    class basic_address_range;
    using address_v4_range = basic_address_range<address_v4>;
    using address_v6_range = basic_address_range<address_v6>;
    class network_v4;
    class network_v6;
    template <class InternetProtocol>
    class basic_endpoint;
    template <class InternetProtocol>
    class basic_resolver_entry;
    template <class InternetProtocol>
    class basic_resolver_results;
    template <class InternetProtocol>
    class basic_resolver;
    class tcp;
    class udp;

}; // namespace mcs::net::ip