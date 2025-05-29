#pragma once

namespace mcs::protocol::ip::concepts
{
    template <typename T>
    concept endpoint = requires(T &e) {
        // typename T::protocol_type;
        e.protocol();
        e.address();
        e.port();
    };

}; // namespace mcs::protocol::ip::concepts