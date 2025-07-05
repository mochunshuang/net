#pragma once

#include "../../io/windows/__iocp_context_base.hpp"
#include "../../io/windows/__iocp_operation.hpp"
#include "../../io/__io_opertion.hpp"
#include "../../../../__third_party.hpp"

#include <algorithm>
#include <cassert>
#include <exception>
#include <print>
#include <system_error>
#include <utility>

namespace mcs::net::services::windows
{

    template <class InternetProtocol>
    struct base_service : io::windows::iocp_context_base
    {
        using socket_type = io::windows::iocp_context_base::socket_type;
        using sockaddr_storage_type =
            io::windows::iocp_context_base::sockaddr_storage_type;
        using accept_operation_context = io::windows::io_operation_accept_context;
        using io_operation_context_base = io::windows::io_operation_context_base;
        using read_operation_context = io_operation_context_base;
        using write_operation_context = io_operation_context_base;
        static constexpr auto invalid_socket_value = INVALID_SOCKET; // NOLINT
        static constexpr auto default_accept_buffer_size =           // NOLINT
            2 * io::windows::ADDRESS_BUFFER_SIZE;
        using connection_type = io::windows::iocp_context_base::rawconnection;

        using endpoint_type = InternetProtocol::endpoint;
        using io_operation_type = io::windows::io_operation_context_base;

        template <typename Sndr, typename Revr>
        struct state : io::windows::iocp_operation<state<Sndr, Revr>>
        {
            static constexpr auto io_type = Sndr::type; // NOLINT
            using operation_state_concept = ::mcs::execution::operation_state_t;

            base_service &service;                          // NOLINT
            Revr rcvr;                                      // NOLINT
            Sndr::operation_context_type operation_context; // NOLINT

            constexpr state(Sndr &sndr, Revr &&rcvr) noexcept // NOLINT
                : io::windows::iocp_operation<state<Sndr, Revr>>{operation_context},
                  service{sndr.service}, rcvr{std::forward<Revr>(rcvr)},
                  operation_context{sndr.operation_context}
            {
            }
            ~state() noexcept = default;
            state(state &&) = delete;
            state(const state &) = delete;
            state &operator=(state &&) = delete;
            state &operator=(const state &) = delete;

            void start() & noexcept
            {
                if constexpr (io_type == io::io_type::IO_ACCEPT)
                {
                    if (auto ret =
                            iocp_context_base::post_accept(service.pfn_acceptex(), this);
                        !ret)
                        ::mcs::execution::recv::set_error(std::move(rcvr), ret.error());
                }
                else if constexpr (io_type == io::io_type::IO_READ)
                {
                    if (auto ret = iocp_context_base::post_read(this); !ret)
                        ::mcs::execution::recv::set_error(std::move(rcvr), ret.error());
                }
                else if constexpr (io_type == io::io_type::IO_WRITE)
                {
                    if (auto ret = iocp_context_base::post_write(this); !ret)
                        ::mcs::execution::recv::set_error(std::move(rcvr), ret.error());
                }
                else
                {
                    std::println("check operation_type. io::io_type is IO_UNKNOWN");
                    std::terminate();
                }
            }
            constexpr static void complete(state *self) noexcept
            {
                ::mcs::execution::recv::set_value(std::move(self->rcvr),
                                                  std::move(self->context));
            }
            // NOLINTNEXTLINE
            constexpr static void complete_error(state *self,
                                                 std::error_code code) noexcept
            {
                ::mcs::execution::recv::set_error(std::move(self->rcvr), std::move(code));
            }
        };

        template <io::io_type io_type>
        struct sender
        {
            using operation_context_type =
                std::conditional_t<io_type == io::io_type::IO_ACCEPT,
                                   ::mcs::net::io::windows::io_operation_accept_context,
                                   ::mcs::net::io::windows::io_operation_context_base>;
            using sender_concept = ::mcs::execution::sender_t;
            using completion_signatures = ::mcs::execution::completion_signatures<
                ::mcs::execution::set_value_t(operation_context_type),
                ::mcs::execution::set_error_t(std::error_code)>;

            using indices_for = std::index_sequence_for<>;
            static constexpr auto type = io_type; // NOLINT

            template <::mcs::execution::receiver Recv>
            constexpr auto connect(Recv recv) noexcept -> state<sender, Recv>
            {
                return {*this, std::move(recv)};
            }
            base_service &service; // NOLINT
            operation_context_type operation_context;
        };
        static_assert(::mcs::execution::sender<sender<io::io_type::IO_ACCEPT>>);

        [[nodiscard]] auto iocp_handle() const noexcept // NOLINT
        {
            return iocp_;
        }
        [[nodiscard]] auto pfn_acceptex() const noexcept // NOLINT
        {
            return pfnAcceptEx_;
        }

        [[nodiscard]] constexpr auto make_accept( // NOLINT
            accept_operation_context context) noexcept
        {
            return sender<io::io_type::IO_ACCEPT>{*this, std::move(context)};
        }
        [[nodiscard]] constexpr auto make_read( // NOLINT
            read_operation_context context) noexcept
        {
            return sender<io::io_type::IO_READ>{*this, std::move(context)};
        }
        [[nodiscard]] constexpr auto make_write( // NOLINT
            write_operation_context context) noexcept
        {
            return sender<io::io_type::IO_WRITE>{*this, std::move(context)};
        }

        [[nodiscard]] constexpr auto make_connection( // NOLINT
            accept_operation_context context) noexcept
        {
            return make_accept(context) |
                   ::mcs::execution::then([this](auto new_ctx) noexcept {
                       return connection_type{new_ctx.socket, connection_info(new_ctx)};
                   });
        }

        [[nodiscard]] constexpr auto make_raw_socket( // NOLINT
            const endpoint_type &endpoint) const noexcept -> socket_type
        {
            auto ret = ::WSASocketW(
                endpoint.protocol().family(), endpoint.protocol().type(),
                endpoint.protocol().protocol(), nullptr, 0, WSA_FLAG_OVERLAPPED);
            assert(ret != INVALID_SOCKET);
            return ret;
        }

        [[nodiscard]] constexpr auto connection_info(
            const io::windows::io_operation_context_base &context) const noexcept
        {
            return iocp_context_base::parse_connection(pfnGetAcceptExSockaddrs_,
                                                       context.wsabuf.buf);
        }

        constexpr explicit base_service()
        {
            //
            // NOTE: SocKet的API WSAStartup后才能使用
            //  初始化Winsock. 如果成功， WSAStartup 函数返回零。
            if (WSADATA wsaData; 0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData))
                throw std::system_error(::WSAGetLastError(), std::system_category());
            try
            {
                // 完成端口
                iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, {}, {});
                if (iocp_ == nullptr || iocp_ == INVALID_HANDLE_VALUE)
                    throw std::system_error(static_cast<int>(::GetLastError()),
                                            std::system_category());

                socket_type tmp_sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

                pfnAcceptEx_ = iocp_context_base::get_acceptex_ptr(tmp_sock);
                pfnGetAcceptExSockaddrs_ =
                    iocp_context_base::get_accpetex_sockaddrs_ptr(tmp_sock);
                ::closesocket(tmp_sock);
                assert(pfnAcceptEx_ != nullptr);
                assert(pfnGetAcceptExSockaddrs_ != nullptr);
            }
            catch (...)
            {
                std::println("********base_service Construction [failed]********");
                deinit();
                std::rethrow_exception(std::current_exception());
            }
        }

        // TODO(mcs): 需要绑定线程池资源的run接口。虽然base_service可以引用传递+pool解决
        void run() noexcept
        {
            stoped_.store(false, std::memory_order_relaxed);
            while (true)
            {
                DWORD bytes_transferred = 0;
                ULONG_PTR completion_key = 0;
                OVERLAPPED *overlapped = nullptr;

                DWORD last_error = (TRUE == ::GetQueuedCompletionStatus(
                                                iocp_, &bytes_transferred,
                                                &completion_key, &overlapped, INFINITE))
                                       ? ERROR_SUCCESS
                                       : ::GetLastError();

                if (stoped_.load(std::memory_order_relaxed))
                {
                    std::println("base_service shutdown....: [thead_id: {}]",
                                 std::this_thread::get_id());
                    break;
                }
                if (overlapped == nullptr)
                {
                    if (last_error == ERROR_ABANDONED_WAIT_0 ||
                        last_error == ERROR_INVALID_HANDLE)
                    {
                        std::println(
                            "overlapped == nullptr and serious error to shutdown: {}",
                            last_error);
                        break;
                    }
                    std::println(
                        "overlapped == nullptr and GetQueuedCompletionStatus error: {}",
                        last_error);
                    continue;
                }

                auto *op = static_cast<io::windows::iocp_operation_base *>(overlapped);
                if (last_error != ERROR_SUCCESS)
                    op->callbacks.complete_error(
                        op, std::error_code(static_cast<int>(::GetLastError()),
                                            std::system_category()));
                switch (op->op_type)
                {
                case io::io_type::IO_ACCEPT: {
                    auto &accept_context =
                        static_cast<io::windows::io_operation_accept_context &>(
                            op->context);
                    socket_type client_socket = accept_context.socket;
                    //  UPDATE_ACCEPT_CONTEXT
                    if (SOCKET_ERROR ==
                        ::setsockopt(
                            client_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                            reinterpret_cast<char *>(&accept_context.listen_socket),
                            sizeof(accept_context.listen_socket)))
                    {
                        DWORD err = ::WSAGetLastError();
                        ::closesocket(client_socket); // TODO(mcs): 交给外边处理吗？
                        op->callbacks.complete_error(
                            op, std::error_code(static_cast<int>(err),
                                                std::system_category()));
                        continue;
                    }

                    if (nullptr ==
                        ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
                                                 iocp_, 0, 0))
                    {
                        op->callbacks.complete_error(
                            op, std::error_code(static_cast<int>(::GetLastError()),
                                                std::system_category()));
                        continue;
                    }
                    op->context.bytes_transferred = bytes_transferred;
                    op->callbacks.complete(op);
                }
                break;
                case io::io_type::IO_READ:
                case io::io_type::IO_WRITE:
                    op->context.bytes_transferred = bytes_transferred;
                    op->callbacks.complete(op);
                    break;
                default:
                    std::println("io::io_type: [ {} ] can not handle",
                                 static_cast<int>(op->op_type));
                    std::terminate();
                }
            }
        }

        constexpr void shutdown(std::size_t thead_count = 1) noexcept
        {
            if (bool expected = false; stoped_.compare_exchange_strong(
                    expected, true, std::memory_order_release, std::memory_order_relaxed))
            {
                std::println("shutdown thead_count: [ {} ]", thead_count);
                for (std::size_t i = 0; i < thead_count; ++i)
                {
                    ::PostQueuedCompletionStatus(iocp_, 0, 0, nullptr);
                }
            }
        }
        [[nodiscard]] constexpr bool is_stopped() const noexcept // NOLINT
        {
            return stoped_.load(std::memory_order_relaxed);
        }

        // NOLINTNEXTLINE
        static constexpr auto get_sockaddr_storage(const endpoint_type &endpoint) noexcept
            -> std::optional<sockaddr_storage_type>
        {
            sockaddr_storage_type storage{};
            if (endpoint.protocol().family() == AF_INET)
            {
                auto &ipv4_addr = reinterpret_cast<sockaddr_in &>(storage.addr);
                ipv4_addr.sin_family = AF_INET;
                ipv4_addr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 注意用 htonl 转换
                ipv4_addr.sin_port = ::htons(endpoint.port());
                storage.addr_len = sizeof(sockaddr_in);
                return storage;
            }
            if (endpoint.protocol().family() == AF_INET6)
            {
                auto &ipv6_addr = reinterpret_cast<sockaddr_in6 &>(storage.addr);
                ipv6_addr.sin6_family = AF_INET6;
                ipv6_addr.sin6_addr = in6addr_any; // IPv6 通配地址
                ipv6_addr.sin6_port = ::htons(endpoint.port());
                storage.addr_len = sizeof(sockaddr_in6);
                return storage;
            }
            return std::nullopt;
        }

        constexpr static void close_service(socket_type listen_socket) noexcept // NOLINT
        {
            assert(listen_socket != invalid_socket_value);
            ::CancelIoEx(reinterpret_cast<HANDLE>(listen_socket), nullptr);
            ::closesocket(listen_socket);
            std::println("close_service ok, [listen_socket: {}]", listen_socket);
        }
        constexpr static void close_socket(socket_type client_socket) noexcept // NOLINT
        {
            assert(client_socket != invalid_socket_value);
            ::CancelIoEx(reinterpret_cast<HANDLE>(client_socket), nullptr);
            ::closesocket(client_socket);
            std::println("close_socket ok, [client_socket: {}]", client_socket);
        }
        constexpr static void close_connection(connection_type conn) noexcept // NOLINT
        {
            close_socket(conn.socket);
        }

        constexpr auto start_service(const endpoint_type &endpoint) noexcept // NOLINT
            -> std::optional<socket_type>
        {
            auto listen_socket = make_raw_socket(endpoint);
            try
            {
                if (listen_socket == INVALID_SOCKET)
                    throw std::system_error(::WSAGetLastError(), std::system_category());

                auto storage = get_sockaddr_storage(endpoint);
                if (not storage.has_value())
                    throw std::runtime_error("Unsupported address family");
                auto [addr, addr_len] = storage.value();
                if (::bind(listen_socket, reinterpret_cast<sockaddr *>(&addr),
                           addr_len) == SOCKET_ERROR)
                    throw std::system_error(::WSAGetLastError(), std::system_category());

                if (::listen(listen_socket, SOMAXCONN) == SOCKET_ERROR)
                    throw std::system_error(::WSAGetLastError(), std::system_category());

                // iocp_ bind to listen_socket
                if (nullptr ==
                    ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket),
                                             iocp_, {}, {}))
                    throw std::system_error(static_cast<int>(::GetLastError()),
                                            std::system_category());

                std::println("service start ok, [port: {},listen_socket: {}]",
                             endpoint.port(), listen_socket);
                return listen_socket;
            }
            catch (const std::system_error &e)
            {
                std::println("********base_service Construction [failed]********");
                std::println("System error: [code: {}], [message: {}]", e.code().value(),
                             e.what());
            }
            catch (const std::runtime_error &e)
            {
                std::println("********base_service Construction [failed]********");
                std::println("Runtime error: [message: {}]", e.what());
            }
            catch (...)
            {
                std::println("********base_service Construction [failed]********");
                std::println("Unknown error occurred");
            }
            ::closesocket(listen_socket);
            return std::nullopt;
        }

        constexpr ~base_service() noexcept
        {
            deinit();
        }
        base_service(const base_service &) = delete;
        base_service(base_service &&) = delete;
        base_service &operator=(const base_service &) = delete;
        base_service &operator=(base_service &&) = delete;

      private:
        HANDLE iocp_{INVALID_HANDLE_VALUE};
        LPFN_ACCEPTEX pfnAcceptEx_{};
        LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_{};
        std::atomic<bool> stoped_{true};

        constexpr void deinit() noexcept
        {
            shutdown();
            if (iocp_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(iocp_);
                iocp_ = INVALID_HANDLE_VALUE;
            }
            ::WSACleanup();
        }
    };
}; // namespace mcs::net::services::windows
