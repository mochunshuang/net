
#pragma once

#include "../../../protocol/__ip.hpp"
#include "../../../../__third_party.hpp"
#include "../../../taps/io/pool/buffer_object_pool.hpp"
#include "../../../taps/io/pool/__pool_object_guard.hpp"
#include <array>
#include <memory>
#include <print>
#include <cassert>
#include <cstddef>
#include <string>
#include <thread>
#include <vector>
#include <unordered_set>

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
            if (auto handle = dependent_service_.start_service(cig.local_endpoint);
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
            // NOTE: 析构的时候，不需要重复 close_service
            if (listen_socket_ != BaseService::invalid_socket_value)
            {
                dependent_service_.close_service(listen_socket_);
                listen_socket_ = BaseService::invalid_socket_value;
            }
        }

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
        http_service &operator=(http_service &&other) noexcept
        {
            if (this != &other)
            {
                dependent_service_ = other.dependent_service_;
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

            // NOTE: 开始可以发送和接收消息？
            // NOTE: 必须是一个sender。必须有线程资源执行协议解析
            // NOTE: 定义Sender 发送 一个http解析好的对象
            // NOTE: operation : 一直 start dependent_service_
            // 进行读操作，直到可以产生对象

            // NOTE: 如何理解长连接？
            // NOTE: 抽象统一处理 HTTP请求，需要 http_connect池 都走同一个通道
            // NOTE: 统一写也是一样的？ 需要一个 system 来管理连接池吗？
            // NOTE: 肯定是需要的。这样就可以 从 service 对象
            // 获取请求了。无状态调用链处理

            // NOTE: 还需要两阶段处理吗？

            auto aync_read(char *data, std::size_t max_len) noexcept // NOLINT
            {
                namespace ex = mcs::execution;
                return [](auto *self, char *data,
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

            base_connect_type dependentConnect() const noexcept
            {
                return dependent_connect_;
            }

          private:
            base_connect_type dependent_connect_;
            BaseService &dependent_service_; // NOLINT

            // NOTE: 成http_frame 的过程是否 可以放在 http_operation 中？
            // NOTE: 成帧乱序安全吗？核心要求一个 coonect 的 req,res 各自顺序安全
            // NOTE: 也就是说，你顺序的发送 req，一定保证 req的完成通知是顺序的
            // std::mutex mutex_; //NOTE: 不能移动。就不能做 sndr的发送值
            std::unordered_set<std::unique_ptr<completed_event>>
                read_completed_events_; // NOLINT
            operation_id next_ = 0;     // 下一个期望的ID
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
            return [](auto &service, auto listen_socket,
                      auto endpoint) -> ::mcs::execution::task<http_connection> {
                std::println(">>> task start thead_id: {}", std::this_thread::get_id());
                mcs::net::io::pool::pool_object_guard object{accept_buffer_pool()};
                auto &buffer = object.data()->buffer;
                accept_operation_context op{
                    io_operation_context_base{
                        service.make_raw_socket(endpoint),
                        {.len = BaseService::default_accept_buffer_size, .buf = buffer}},
                    listen_socket};
                auto rawconnect = co_await service.make_connection(op);
                std::println(">>> task end thead_id: {}", std::this_thread::get_id());
                co_return http_connection{std::move(rawconnect), service};
            }(dependent_service_, listen_socket_, start_config_.local_endpoint);
        }
        // TODO(mcs): 协议解析需要 start.
        // 需要单独的线程吗？。需要看看statci_线程池部分的工作
        // NOTE: 都是非递归锁。如何解决。如何避免循环的出现？

        auto accept_requres()
        {
            // NOTE: 返回一个 request 对象。
            // NOTE: request 是 move_only的对象。当处理完毕，释放connect
            // NOTE: 如果 connect 是长连接，放到connect池中，否则释放所有相关资源
            // NOTE: 统一。 http service 的请求都经过这里
            // NOTE: 为了的事件通知和转发前处理必须经过这里、
            // NOTE: 可以根据业务和责任类型，分到其他类中
            // NOTE: 需要一个队列来处理了？当生产者和消费者大于消费者？
            // NOTE:
            // 不需要，因为已经有完成队列了。目前确定了不投递连接操作，也能建立连接中
            // NOTE: 应该是 range 生产的协程。阻塞？还是异步
            // NOTE: 还是需要队列，因为完成队列的结果不能直接使用

            // NOTE: make_http_connection() -> queue -> seletc_one -> requrest_obj
            // NOTE: while 循环 放在 queue?
            // NOTE: 是否两阶段。协议头部 + 协议数据
            // NOTE: 如果数据大小确定，应该单个阶段解析就OK
            // NOTE: 如果数据大小不确定，是长连接。应该共享头部 + 新数据

            // NOTE: 完成顺序没用顺序如何解决？
            // NOTE: http 服务 本质是 处理 http_req ,http_res 的线程
            // NOTE: 协议解析 是否独立线程处理？开线槽太多没用
            // NOTE: 调度 交给 BaseService 的线程资源吗？
            // NOTE: 目前 BaseService 是单线程处理所有。 事件的投递，非多线程
            // NOTE: 如果 BaseService 即IO线程池是多线程，做驱动如何？
            // NOTE: 如何 手动标记 线程池 切换？

            // NOTE: 分离启动，才是对的。这就是切换。 任务图通过 Sndr编写
            // NOTE: start() 函数就是占用CPU时间片的 过程
            // NOTE: 具体的 start 由 http_hanlde_sndr 确定

            // NOTE: Sndr 提供数据 + 处理前面sndr 提供的数据

            // NOTE: 无状态则不需要 锁。 确定的数据data，一次协议解析，应该设计成无锁的

            // NOTE: 长连接 + 多次数据，大数据。要两次解析的。局部变量不需要 锁
        }

      private:
        BaseService &dependent_service_;                               // NOLINT
        socket_type listen_socket_{BaseService::invalid_socket_value}; // NOLINT
        connect_count_config init_config_;                             // NOLINT
        start_config start_config_;                                    // NOLINT
    };

}; // namespace mcs::net::services::http