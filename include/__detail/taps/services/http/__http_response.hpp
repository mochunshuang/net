
#pragma once

#include "./__http_body_writer.hpp"

#include <string>
#include <unordered_map>

namespace mcs::net::services::http
{
    struct http_response
    {
        int status_code;
        std::string reason_phrase;
        std::unordered_map<std::string, std::string> headers;
        http_body_writer body;
    };
}; // namespace mcs::net::services::http