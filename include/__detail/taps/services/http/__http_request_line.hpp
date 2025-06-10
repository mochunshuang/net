
#pragma once

#include <string>

namespace mcs::net::services::http
{
    struct http_request_line
    {
        std::string method;
        std::string uri;
        std::string version;
    };
}; // namespace mcs::net::services::http