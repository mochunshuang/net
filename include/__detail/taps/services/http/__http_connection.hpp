
#pragma once

#include <utility>
namespace mcs::net::services::http
{
    template <typename Service>
    struct http_connection
    {
        using socket_type = decltype(std::declval<Service>().make_raw_socket());
    };
}; // namespace mcs::net::services::http