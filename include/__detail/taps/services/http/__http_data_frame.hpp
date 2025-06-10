
#pragma once

#include <vector>

namespace mcs::net::services::http
{
    struct http_data_frame
    {
        std::vector<std::uint8_t> data;
        bool is_final = false; // 是否为最后一个数据块
    };
}; // namespace mcs::net::services::http