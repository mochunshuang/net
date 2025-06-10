
#pragma once

#include "./__http_request_line.hpp"
#include "./__http_status_line.hpp"

#include <unordered_map>
#include <variant>

namespace mcs::net::services::http
{
    struct http_mate_frame
    {
        using start_line_type =
            std::variant<std::monostate, http_request_line, http_status_line>;
        using headers_type = std::unordered_map<std::string, std::string>;

        start_line_type start_line;
        headers_type headers;
    };
}; // namespace mcs::net::services::http