#pragma once
#include "./__typefwd.hpp"

namespace mcs::net::ip
{
    class tcp
    {
      public:
        // types:
        using endpoint = basic_endpoint<tcp>;
        using resolver = basic_resolver<tcp>;
        // using socket = basic_stream_socket<tcp>;
        // using acceptor = basic_socket_acceptor<tcp>;
        // using iostream = basic_socket_iostream<tcp>;
        class no_delay;
        // static members:
        static constexpr tcp v4() noexcept;
        static constexpr tcp v6() noexcept;
        tcp() = delete;
    };
}; // namespace mcs::net::ip