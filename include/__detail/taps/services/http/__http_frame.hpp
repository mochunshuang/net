
#pragma once

#include "./__http_frame_type.hpp"
#include "./__http_mate_frame.hpp"
#include "./__http_data_frame.hpp"

namespace mcs::net::services::http
{
    struct http_frame
    {
        http_frame_type type{http_frame_type::META};
        http_mate_frame mate;
        http_data_frame data;
    };
}; // namespace mcs::net::services::http