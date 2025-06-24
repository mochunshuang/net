
#pragma once

#include <utility>
#include "../../../../__third_party.hpp"
#include "../../io/__io_opertion.hpp"

namespace mcs::net::services::http
{
    // NOTE: 所以service 依赖 base_io_service
    //  NOTE: 将 http_connection 作为一个 Service 如何？
    // NOTE: http_connection 的请求和响应 必须保证顺序性。iocp是不保证的
    // NOTE: 那么 任意。 必须按序号发送？
    template <typename HttpService>
    struct http_connection
    {
        using socket_type = decltype(std::declval<HttpService>().make_raw_socket());

        //NOTE: 1.  先和上游 建立连接。得到socket 代码对象

        // NOTE: on_read / on_write => sender / receiver 异步模型

        // NOTE: 解析应该分成两个阶阶段
        // NOTE: 元信息： path + http_data 处理类型<=> handler

        template <typename Sndr, typename Revr>
        struct http_operation
        {
            static constexpr auto io_type = Sndr::type; // NOLINT
            using operation_state_concept = ::mcs::execution::operation_state_t;

            void start() & noexcept
            {
                //
            }
        };
        template <io::io_type io_type>
        struct http_sender
        {
            using sender_concept = ::mcs::execution::sender_t;
            // NOTE: 是否统一。
            // NOTE: 协议解析 是否是一个 sender 应该是的
            // NOTE: 看来需要回调
        };

        // NOTE: API的设计 method + uri + head + body
        // NOTE: 2阶段的解析 。 头部确定接受数据的方式，定点解决
        // NOTE: 如果数据是定长的，好解决，直接封装成一个 framers
        // NOTE:
        // 如果数据不定长，多次，实时。数据片段如何处理？是否变成固定长度接受？但是肯定是不完整不能一次解决的

        auto send_messages(auto raw_data) {}
        auto send_framers(auto messages) {}
        auto send_bytestream(auto framers) {}

        HttpService &up_service;
    };
}; // namespace mcs::net::services::http