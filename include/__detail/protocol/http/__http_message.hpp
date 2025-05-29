#pragma once

#include "./__start_line.hpp"
#include "./__field_line.hpp"
#include "./__message_body.hpp"
#include <optional>
#include <vector>

namespace mcs::protocol::http
{
    /**
     * @brief
     异步I/O接收 → 解析start-line → 解析headers → 处理请求 → 生成响应
      ↑           ｜错误立即返回        ｜校验元数据        ｜分块流式传输
      └───────────┴──────────────────────┴───────────────────┘
     */
    // HTTP-message = start-line CRLF *( field-line CRLF ) CRLF [ message-body ]
    struct HTTPMessage
    {
        StartLine start_line;
        std::vector<FieldLine> field_lines;
        std::optional<MessageBody> message_body;
    };
}; // namespace mcs::protocol::http