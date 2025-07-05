
#pragma once

#include "../../../protocol/__ip.hpp"
#include "../../../../__third_party.hpp"
#include "../../../taps/io/pool/buffer_object_pool.hpp"
#include "../../../taps/io/pool/__pool_object_guard.hpp"
#include <array>
#include <print>
#include <cassert>
#include <cstddef>
#include <thread>
#include <vector>

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

        explicit http_service() = delete;
        explicit http_service(BaseService &base,
                              connect_count_config init_config) noexcept
            : dependent_service_{base}, init_config_{init_config}
        {
            assert(init_config.init_connect_count <= init_config.adjustable_count);
            assert(init_config.adjustable_count <= init_config.max_connect_count);
        }

        ~http_service() noexcept = default;
        http_service(http_service &&other) noexcept
            : dependent_service_(other.dependent_service_),
              listen_socket_(
                  std::exchange(other.listen_socket_, BaseService::invalid_socket_value))
        {
        }
        http_service &operator=(http_service &&other) noexcept = delete;
        http_service(const http_service &) = delete;
        http_service &operator=(const http_service &) = delete;

        // TODO(mcs): http_service 应该管理多个 http_connect
        struct connection_resource
        {
            std::array<char, BaseService::default_accept_buffer_size> buffer;
        };

        struct http_connection
        {
            using base_connect_type = BaseService::connection_type;
            explicit http_connection(base_connect_type &&c, BaseService &servce) noexcept
                : dependent_connect_{std::move(c)}, dependent_service_{servce}
            {
            }
            // NOTE: 核心的核心是，提供执行体，给 BaseService::io线程执行
            // NOTE: 依赖顺序，因为IOCP的完成通知是乱序的，需要额外操作
            // NOTE: 然后唤醒或通知 http_service 有一个 http_res 或 http_req 可以提供
            // NOTE: 这里是否是最底层IO沟通媒介？ 我想是的，这样最好，最直接
            using operation_id = std::size_t;
            using operation_data = std::vector<char>;
            struct completed_event
            {
                operation_id id{0};
                operation_data completed_data;
            };

            auto aync_read(char *data, std::size_t max_len) noexcept // NOLINT
            {
                namespace ex = mcs::execution;
                return [](auto *self, auto *data,
                          std::size_t max_len) -> ex::task<std::size_t> {
                    auto new_ctx = co_await self->dependent_service_.make_read(
                        {self->dependent_connect_.socket,
                         {.len = static_cast<::ULONG>(max_len), .buf = data}});
                    co_return new_ctx.bytes_transferred;
                }(this, data, max_len);
            }
            // NOLINTNEXTLINE
            auto aync_write(operation_data &&data) noexcept -> mcs::execution::sender auto
            {
                namespace ex = mcs::execution;
                return [](auto *self, operation_data data) -> ex::task<std::size_t> {
                    auto new_ctx = co_await self->dependent_service_.make_write(
                        {self->dependent_connect_.socket,
                         {.len = static_cast<::ULONG>(data.size()), .buf = data.data()}});
                    co_return new_ctx.bytes_transferred;
                }(this, std::move(data));
            }

            base_connect_type dependent_connect() const noexcept // NOLINT
            {
                return dependent_connect_;
            }

          private:
            base_connect_type dependent_connect_; // NOLINT
            BaseService &dependent_service_;      // NOLINT
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
        // NOLINTNEXTLINE
        auto make_http_connection() noexcept -> ::mcs::execution::sender auto
        {
            using accept_operation_context = BaseService::accept_operation_context;
            using io_operation_context_base = BaseService::io_operation_context_base;
            return [](auto *slef,
                      auto endpoint) -> ::mcs::execution::task<http_connection> {
                std::println(">>> task start thead_id: {}", std::this_thread::get_id());
                mcs::net::io::pool::pool_object_guard object{accept_buffer_pool()};
                auto &buffer = object.data()->buffer;
                accept_operation_context op{
                    io_operation_context_base{
                        slef->dependent_service().make_raw_socket(endpoint),
                        {.len = BaseService::default_accept_buffer_size, .buf = buffer}},
                    slef->listen_socket()};
                auto rawconnect = co_await slef->dependent_service().make_connection(op);
                std::println(">>> task end thead_id: {}", std::this_thread::get_id());
                co_return http_connection{std::move(rawconnect),
                                          slef->dependent_service()};
            }(this, start_config_.local_endpoint);
        }
        void close_http_connection(http_connection conn) noexcept // NOLINT
        {
            dependent_service_.close_connection(conn.dependent_connect());
        }

        auto accept_requres() {}

        bool start(const start_config &cig) noexcept
        {
            assert(listen_socket_ == BaseService::invalid_socket_value);
            if (auto handle = dependent_service_.start_service(cig.local_endpoint);
                handle.has_value())
            {
                listen_socket_ = *handle;
                return true;
            }
            return false;
        }
        void stop() noexcept
        {
            if (listen_socket_ != BaseService::invalid_socket_value)
            {
                dependent_service_.close_service(listen_socket_);
                listen_socket_ = BaseService::invalid_socket_value;
            }
        }
        BaseService &dependent_service() const noexcept // NOLINT
        {
            return dependent_service_;
        }
        socket_type listen_socket() const // NOLINT
        {
            return listen_socket_;
        }

      private:
        BaseService &dependent_service_;                               // NOLINT
        socket_type listen_socket_{BaseService::invalid_socket_value}; // NOLINT
        connect_count_config init_config_;                             // NOLINT
        start_config start_config_;                                    // NOLINT
    };

}; // namespace mcs::net::services::http