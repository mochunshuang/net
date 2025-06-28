
#pragma once

#include "../../../protocol/__ip.hpp"
#include "../../../../__third_party.hpp"
#include "../../../taps/io/pool/buffer_object_pool.hpp"
#include <array>
#include <print>
#include <cassert>
#include <cstddef>
#include <string>

namespace mcs::net::services::http
{

    // NOTE: http 基于 tcp
    template <typename BaseService>
    struct http_service
    {
        using endpoint_type = ::mcs::protocol::ip::tcp::endpoint;
        using socket_type = BaseService::socket_type;

        using read_operation_context = BaseService::read_operation_context;
        using write_operation_context = BaseService::write_operation_context;

        struct connect_count_config
        {
            size_t init_connect_count{1024}; // NOLINT
            size_t adjustable_count{init_connect_count};
            size_t max_connect_count{init_connect_count};
        };
        struct start_config
        {
            // LocalSpecifier,RemoteSpecifier,TransportProperties,SecurityProperties
            endpoint_type local_endpoint;
        };

        bool start(const start_config &cig) noexcept
        {
            assert(listen_socket_ == BaseService::invalid_socket_value);
            if (auto handle = up_service_.start_service(cig.local_endpoint);
                handle.has_value())
            {
                listen_socket_ = *handle;
                // TODO(mcs): 发生并发连接？
                return true;
            }
            return false;
        }
        bool stop() noexcept
        {
            if (listen_socket_ != BaseService::invalid_socket_value)
                up_service_.close_service(listen_socket_);
        }

        explicit http_service() = delete;
        explicit http_service(BaseService &base,
                              connect_count_config init_config) noexcept
            : up_service_{base}, init_config_{init_config}
        {
            assert(init_config.init_connect_count <= init_config.adjustable_count);
            assert(init_config.adjustable_count <= init_config.max_connect_count);
        }

        ~http_service() noexcept
        {
            stop();
        }
        http_service(http_service &&other) noexcept
            : up_service_(other.up_service_),
              listen_socket_(
                  std::exchange(other.listen_socket_, BaseService::invalid_socket_value))
        {
        }
        http_service &operator=(http_service &&other) noexcept
        {
            if (this != &other)
            {
                up_service_ = other.up_service_;
                listen_socket_ = std::exchange(other.listen_socket_,
                                               BaseService::invalid_socket_value);
            }
            return *this;
        }
        http_service(const http_service &) = delete;
        http_service &operator=(const http_service &) = delete;

        // TODO(mcs): http_service 应该管理多个 http_connect
        struct connection_resource
        {
            std::array<char, BaseService::default_accept_buffer_size> buffer;
        };

        auto *allocate_connection_resource() noexcept
        {
            //
        }

        struct http_connection
        {
            using base_connect_type = BaseService::connection_type;
            explicit http_connection(base_connect_type &&c) noexcept
                : connect_{std::move(c)}
            {
            }

            // NOTE: 开始可以发送和接收消息？
            // NOTE: 必须是一个sender。必须有线程资源执行协议解析

          private:
            base_connect_type connect_;
        };

        struct accept_buffer
        {
            char buffer[BaseService::default_accept_buffer_size]; // NOLINT
        };

        static auto &accept_buffer_pool() noexcept // NOLINT
        {
            using pool_type = mcs::net::io::pool::buffer_object_pool<accept_buffer>;
            static pool_type pool{pool_type::chunk::block_count};
            return pool;
        }

        auto make_http_connection() noexcept // NOLINT
            -> ::mcs::execution::sender auto
        {
            using accept_operation_context = BaseService::accept_operation_context;
            using io_operation_context_base = BaseService::io_operation_context_base;

            return [](auto &service, auto listen_socket,
                      auto endpoint) -> ::mcs::execution::lazy<http_connection> {
                // TODO(mcs): BUFFER池
                accept_buffer *buf_obj = accept_buffer_pool().allocate();
                auto &buffer = buf_obj->buffer;
                accept_operation_context op{
                    io_operation_context_base{
                        service.make_raw_socket(endpoint),
                        {.len = BaseService::default_accept_buffer_size, .buf = buffer}},
                    listen_socket};
                auto rawconnect = co_await service.make_connection(op);

                std::println("New connection: {}", rawconnect.info.to_string());
                std::println("Remote IP: {}", rawconnect.info.remote.ip_address);
                std::println("Remote Port: {}", rawconnect.info.remote.port);
                std::println("Local Port: {}", rawconnect.info.local.port);

                {
                    char buffer[4096];
                    auto ctx = co_await service.make_read(
                        {rawconnect.socket, {.len = 4096, .buf = buffer}});
                    std::println("bytes_transferred: {} \n read: {}",
                                 ctx.bytes_transferred,
                                 std::string(buffer, ctx.bytes_transferred));
                }
                {
                    // 构造响应
                    std::string responseBody = "Hello World!";
                    std::string response = "HTTP/1.1 200 OK\r\n"
                                           "Content-Type: text/plain\r\n"
                                           "Content-Length: " +
                                           std::to_string(responseBody.length()) +
                                           "\r\n"
                                           "Connection: " +
                                           (false ? "keep-alive" : "close") +
                                           "\r\n"
                                           "\r\n" // 头与正文的空行
                                           + responseBody;
                    auto ctx = co_await service.make_write(
                        {rawconnect.socket,
                         {.len = static_cast<::ULONG>(response.size()),
                          .buf = response.data()}});
                    std::println("write bytes_transferred: {} , response.size(): {} ",
                                 ctx.bytes_transferred, response.size());
                }

                accept_buffer_pool().deallocate(buf_obj);

                // NOTE: 测试

                co_return http_connection{std::move(
                    rawconnect)}; // NOTE: 从这里开始可以切换线程池。// TODO 小心递归锁
            }(up_service_, listen_socket_, start_config_.local_endpoint);

            /*
接下来的读：
           char buffer[4096];
                        auto ctx = co_await service.make_read(
                            {connect.socket, {.len = 4096, .buf = buffer}});
                        std::println("bytes_transferred: {} \n read: {}",
                                     ctx.bytes_transferred,
                                     std::string(buffer, ctx.bytes_transferred));

*/
        }
        // TODO(mcs): 协议解析需要 start.
        // 需要单独的线程吗？。需要看看statci_线程池部分的工作
        // NOTE: 都是非递归锁。如何解决。如何避免循环的出现？

      private:
        BaseService &up_service_;                                      // NOLINT
        socket_type listen_socket_{BaseService::invalid_socket_value}; // NOLINT
        connect_count_config init_config_;                             // NOLINT
        start_config start_config_;                                    // NOLINT
    };

}; // namespace mcs::net::services::http