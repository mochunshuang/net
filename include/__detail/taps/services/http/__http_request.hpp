#pragma once

#include "./__http_body_reader.hpp"

#include <string>
#include <unordered_map>

namespace mcs::net::services::http
{
    struct http_request
    {
        std::string method;
        std::string uri;
        std::string version;
        std::unordered_map<std::string, std::string> headers;
        http_body_reader body;
    };
}; // namespace mcs::net::services::http