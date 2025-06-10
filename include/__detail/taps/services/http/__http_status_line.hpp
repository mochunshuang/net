
#pragma once

#include <string>

namespace mcs::net::services::http
{
    struct http_status_line
    {
        std::string version;
        int status_code;
        std::string reason_phrase;
    };
}; // namespace mcs::net::services::http