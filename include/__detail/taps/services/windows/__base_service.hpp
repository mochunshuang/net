#pragma once

#include "../../io/windows/__iocp_context_base.hpp"
#include "../../io/windows/__iocp_operation.hpp"
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
        using endpoint_type = InternetProtocol::endpoint;

        template <typename Sndr, typename Revr>
        struct state : io::windows::iocp_operation<state<Sndr, Revr>>
        {
            static constexpr auto io_type = Sndr::type; // NOLINT
            using operation_state_concept = ::mcs::execution::operation_state_t;

            base_service &service;                                    // NOLINT
            Revr rcvr;                                                // NOLINT
            io::windows::io_operation_context_base operation_context; // NOLINT

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
                    if (auto ret = iocp_context_base::post_accept(service, this); !ret)
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
            using sender_concept = ::mcs::execution::sender_t;
            using completion_signatures = ::mcs::execution::completion_signatures<
                ::mcs::execution::set_value_t(
                    ::mcs::net::io::windows::io_operation_context_base),
                ::mcs::execution::set_error_t(std::error_code)>;

            using indices_for = std::index_sequence_for<>;
            static constexpr auto type = io_type; // NOLINT

            template <typename Recv>
            constexpr auto connect(Recv recv) noexcept -> state<sender, Recv>
            {
                return {*this, std::move(recv)};
            }
            base_service &service; // NOLINT
            io::windows::io_operation_context_base operation_context;
        };
        static_assert(::mcs::execution::sender<sender<io::io_type::IO_ACCEPT>>);

        [[nodiscard]] auto iocp_handle() const noexcept // NOLINT
        {
            return iocp_;
        }
        [[nodiscard]] auto listen_socket() const noexcept // NOLINT
        {
            return listenSocket_;
        }
        [[nodiscard]] auto pfn_acceptex() const noexcept // NOLINT
        {
            return pfnAcceptEx_;
        }

        [[nodiscard]] constexpr auto make_accept( // NOLINT
            io::windows::io_operation_context_base context) noexcept
        {
            return sender<io::io_type::IO_ACCEPT>{*this, std::move(context)};
        }
        [[nodiscard]] constexpr auto make_read( // NOLINT
            io::windows::io_operation_context_base context) noexcept
        {
            return sender<io::io_type::IO_READ>{*this, std::move(context)};
        }
        [[nodiscard]] constexpr auto make_write( // NOLINT
            io::windows::io_operation_context_base context) noexcept
        {
            return sender<io::io_type::IO_WRITE>{*this, std::move(context)};
        }

        [[nodiscard]] constexpr auto make_rawconnection( // NOLINT
            io::windows::io_operation_context_base context) noexcept
        {
            return make_accept(context) |
                   ::mcs::execution::then([this](auto new_ctx) noexcept {
                       return rawconnection{new_ctx.socket, connection_info(new_ctx)};
                   });
        }

        [[nodiscard]] constexpr auto make_raw_socket() const noexcept // NOLINT
        {
            auto ret = ::WSASocketW(
                endpoint_.protocol().family(), endpoint_.protocol().type(),
                endpoint_.protocol().protocol(), nullptr, 0, WSA_FLAG_OVERLAPPED);
            assert(ret != INVALID_SOCKET);
            return ret;
        }
        [[nodiscard]] constexpr auto family() const noexcept
        {
            return endpoint_.protocol().family();
        }
        [[nodiscard]] constexpr auto connection_info(
            const io::windows::io_operation_context_base &context) const noexcept
        {
            return iocp_context_base::parse_connection(pfnGetAcceptExSockaddrs_,
                                                       context.wsabuf.buf);
        }

        constexpr explicit base_service(endpoint_type endpoint) : endpoint_{endpoint}
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

                // 监听socket
                listenSocket_ = make_raw_socket();
                if (listenSocket_ == INVALID_SOCKET)
                    throw std::system_error(::WSAGetLastError(), std::system_category());

                sockaddr_storage addr = {};
                int addrLen = 0;

                if (endpoint.protocol().family() == AF_INET)
                {
                    // IPv4 处理
                    auto &ipv4_addr = reinterpret_cast<sockaddr_in &>(addr);
                    ipv4_addr.sin_family = AF_INET;
                    ipv4_addr.sin_addr.s_addr = ::htonl(INADDR_ANY); // 注意用 htonl 转换
                    ipv4_addr.sin_port = ::htons(endpoint.port());
                    addrLen = sizeof(sockaddr_in);
                }
                else if (endpoint.protocol().family() == AF_INET6)
                {
                    // IPv6 处理
                    auto &ipv6_addr = reinterpret_cast<sockaddr_in6 &>(addr);
                    ipv6_addr.sin6_family = AF_INET6;
                    ipv6_addr.sin6_addr = in6addr_any; // IPv6 通配地址
                    ipv6_addr.sin6_port = ::htons(endpoint.port());
                    addrLen = sizeof(sockaddr_in6);
                }
                else
                {
                    throw std::runtime_error("Unsupported address family");
                }

                if (::bind(listenSocket_, reinterpret_cast<sockaddr *>(&addr), addrLen) ==
                    SOCKET_ERROR)
                {
                    throw std::system_error(::WSAGetLastError(), std::system_category());
                }

                if (::listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR)
                    throw std::system_error(::WSAGetLastError(), std::system_category());

                pfnAcceptEx_ = iocp_context_base::get_acceptex_ptr(listenSocket_);
                pfnGetAcceptExSockaddrs_ =
                    iocp_context_base::get_accpetex_sockaddrs_ptr(listenSocket_);

                // 将监听socket关联到完成端口
                if (nullptr ==
                    ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket_),
                                             iocp_, {}, {}))
                    throw std::system_error(static_cast<int>(::GetLastError()),
                                            std::system_category());

                std::println("service start ok, [port: {}]", endpoint.port());
            }
            catch (...)
            {
                std::println("********base_service Construction [failed]********");
                deinit();
                std::rethrow_exception(std::current_exception());
            }
        }

        void run() noexcept
        {
            std::println("run service port: [{}] .....", endpoint_.port());
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

                if (finish_.load(std::memory_order_relaxed))
                {
                    std::println("service shutdown....");
                    break;
                }
                if (overlapped == nullptr)
                {
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
                    ::SOCKET client_socket = op->context.socket;
                    // UPDATE_ACCEPT_CONTEXT
                    ::setsockopt(client_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                                 reinterpret_cast<char *>(&listenSocket_),
                                 sizeof(listenSocket_));
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

        constexpr void shutdown() noexcept
        {
            this->finish_.store(true, std::memory_order::memory_order_release);
            ::PostQueuedCompletionStatus(iocp_, 0, 0, nullptr);
        }
        [[nodiscard]] constexpr bool is_stopped() const noexcept // NOLINT
        {
            return finish_.load(std::memory_order_relaxed);
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
        endpoint_type endpoint_;
        HANDLE iocp_{INVALID_HANDLE_VALUE};
        SOCKET listenSocket_{INVALID_SOCKET};
        LPFN_ACCEPTEX pfnAcceptEx_{};
        LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_{};
        std::atomic<bool> finish_;

        constexpr void deinit() noexcept
        {
            if (listenSocket_ != INVALID_SOCKET)
            {
                ::closesocket(listenSocket_);
                listenSocket_ = INVALID_SOCKET;
            }
            if (iocp_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(iocp_);
                iocp_ = INVALID_HANDLE_VALUE;
            }
            ::WSACleanup();
        }
    };
}; // namespace mcs::net::services::windows
