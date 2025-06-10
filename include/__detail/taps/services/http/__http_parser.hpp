
#pragma once

#include <cstddef>

namespace mcs::net::services::http
{
    struct http_parser
    {
        // NOTE: 应该是持续还是单次的呢？ 持续则一个parser对应一个 socket
        // NOTE: 配合 协程？生成数据的 generate?。 然后外部for 循环？
        // NOTE: buf + len 一定完整处理呢？ 不一定是否需要有状态？
        // NOTE: 有没有好的处理方式？
        // NOTE: buf + len 修改成 queue_buf 会更好。这保证每次生成都是有效数据，才返回
        auto operator()(char *buf, std::size_t len) noexcept
        {
            //
        }
    };
}; // namespace mcs::net::services::http