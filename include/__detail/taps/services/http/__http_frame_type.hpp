
#pragma once

#include <cstdint>

namespace mcs::net::services::http
{
    enum http_frame_type : std::uint8_t
    {
        INIT,
        META, // 控制帧 (元数据: 起始行、头部)
        DATA, // 数据帧 (消息体数据块)
        END   // 结束帧 (请求/响应结束)
    };
}; // namespace mcs::net::services::http