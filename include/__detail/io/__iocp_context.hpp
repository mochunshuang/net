#pragma once

#include <algorithm>
#include <atomic>
#include <exception>
#include <iostream>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#if defined(_MSC_VER)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h> // 包含 InetNtop 函数的头文件

#pragma comment(lib, "ws2_32.lib")

#include "./__io_type.hpp"
#include "../../__third_party.hpp"

#include <queue>
#include <vector>
#include <type_traits>
#include <print>

namespace mcs::net::io
{
    namespace iocp
    {
        static constexpr auto MIN_ADDR_LENGTH = sizeof(sockaddr_in) + 16; // NOLINT
        static constexpr std::size_t BUFF_SIZE = 4096;                    // NOLINT

        struct IOCPOperationBase
        {
            ::OVERLAPPED overlapped; // NOLINT
            io_type op_type;         // NOLINT
            ::SOCKET related_socket; // NOLINT
            explicit IOCPOperationBase(io_type type,
                                       ::SOCKET sock = INVALID_SOCKET) noexcept
                : overlapped{}, op_type(type), related_socket(sock)
            {
            }
        };
        static_assert(std::is_standard_layout_v<IOCPOperationBase>,
                      "Must be standard layout");
        static_assert(offsetof(IOCPOperationBase, overlapped) == 0,
                      "OVERLAPPED must be first member");

        struct OperationBase
        {
            using callback_fun_t = void (*)(OperationBase *self) noexcept;
            using callback_error_t = void (*)(OperationBase *self, int code) noexcept;
            callback_fun_t complete{nullptr};
            callback_error_t error{nullptr};
        };

        struct AcceptOp
        {

            IOCPOperationBase base;             // NOLINT
            ::SOCKET client_socket;             // NOLINT
            char buffer[2 * MIN_ADDR_LENGTH]{}; // NOLINT
            OperationBase *parent_op{};         // NOLINT
            ::sockaddr_storage local_addr{};    // NOLINT
            ::sockaddr_storage remote_addr{};   // NOLINT
            int localLen = 0, remoteLen = 0;    // NOLINT
            explicit AcceptOp(SOCKET listen_sock) noexcept
                : base(io_type::IO_ACCEPT, listen_sock), client_socket(INVALID_SOCKET)
            {
            }
            ::OVERLAPPED *getOVERLAPPED() noexcept
            {
                return &base.overlapped;
            }
        };
        static_assert(offsetof(AcceptOp, base) == 0, "Base must be first");
        static_assert(std::is_standard_layout_v<AcceptOp>, "Must be standard layout");
        static_assert(offsetof(AcceptOp, base.overlapped) == 0,
                      "OVERLAPPED must be first member");

        struct ReadOp
        {
            using operation_state_concept = ::mcs::execution::operation_state_t;

            IOCPOperationBase base;       // NOLINT
            char buffer[BUFF_SIZE]{};     // NOLINT
            ::DWORD bytes_transferred{0}; // NOLINT

            explicit ReadOp(::SOCKET sock) noexcept : base(io_type::IO_READ, sock) {}
            ::OVERLAPPED *getOVERLAPPED() noexcept
            {
                return &base.overlapped;
            }
        };
        static_assert(offsetof(ReadOp, base) == 0, "Base must be first");
        static_assert(std::is_standard_layout_v<ReadOp>, "Must be standard layout");
        static_assert(offsetof(ReadOp, base.overlapped) == 0,
                      "OVERLAPPED must be first member");
        struct WriteOp
        {
            using operation_state_concept = ::mcs::execution::operation_state_t;

            IOCPOperationBase base; // NOLINT
            ::WSABUF wsa_buf;       // NOLINT
            ::DWORD bytes_sent{0};  // NOLINT

            explicit WriteOp(::SOCKET sock, char *data, std::size_t len) noexcept
                : base(io_type::IO_WRITE, sock), wsa_buf{static_cast<::ULONG>(len), data}
            {
            }
            ::OVERLAPPED *getOVERLAPPED() noexcept
            {
                return &base.overlapped;
            }
        };
        static_assert(offsetof(WriteOp, base) == 0, "Base must be first");
        static_assert(std::is_standard_layout_v<WriteOp>, "Must be standard layout");
        static_assert(offsetof(WriteOp, base.overlapped) == 0,
                      "OVERLAPPED must be first member");

        struct Connection
        {
            ::SOCKET client_socket;           // NOLINT
            ::sockaddr_storage local_addr{};  // NOLINT
            ::sockaddr_storage remote_addr{}; // NOLINT

            explicit Connection(AcceptOp *acceptOp) noexcept
                : client_socket(acceptOp->client_socket)
            {
                ::memcpy(&local_addr, &acceptOp->local_addr, acceptOp->localLen);
                ::memcpy(&remote_addr, &acceptOp->remote_addr, acceptOp->remoteLen);
            }

            auto async_read() {}

            static constexpr auto GetAddressString = // NOLINT
                [](const ::sockaddr_storage &addr) -> std::string {
                char ipStr[INET6_ADDRSTRLEN]; // NOLINT
                uint16_t port = 0;

                if (addr.ss_family == AF_INET)
                {
                    auto *sin = reinterpret_cast<const sockaddr_in *>(&addr); // NOLINT
                    ::inet_ntop(AF_INET, &sin->sin_addr, ipStr, sizeof(ipStr));
                    port = ::ntohs(sin->sin_port);
                }
                else if (addr.ss_family == AF_INET6)
                {
                    auto *sin6 = reinterpret_cast<const sockaddr_in6 *>(&addr); // NOLINT
                    ::inet_ntop(AF_INET6, &sin6->sin6_addr, ipStr, sizeof(ipStr));
                    port = ::ntohs(sin6->sin6_port);
                }
                else
                {
                    return "Unknown Address Family";
                }
                return std::string(ipStr) + ":" + std::to_string(port);
            };
        };

        static void delete_operation(IOCPOperationBase *op, int code) noexcept
        {
            if (op->op_type == io_type::IO_ACCEPT)
            {
                auto *o = reinterpret_cast<AcceptOp *>(op);
                o->parent_op->error(o->parent_op, code);
                return;
            }
            if (op->op_type == io_type::IO_READ)
            {
                delete reinterpret_cast<ReadOp *>(op);
                return;
            }
            if (op->op_type == io_type::IO_WRITE)
            {
                delete reinterpret_cast<WriteOp *>(op);
                return;
            }
        }
    }; // namespace iocp

    namespace adaptors
    {
        struct PreConnection;
    };

    struct iocp_context
    {
      public:
        iocp_context()
            : iocp_(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0)),
              listenSocket_(INVALID_SOCKET)
        {
            ::WSADATA wsaData;
            if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            if (iocp_ == nullptr)
            {
                throw std::system_error(static_cast<int>(::GetLastError()),
                                        std::system_category());
            }

            listenSocket_ = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0,
                                         WSA_FLAG_OVERLAPPED);
            if (listenSocket_ == INVALID_SOCKET)
            {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            ::sockaddr_in addr{};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = ::htons(8080);

            if (::bind(listenSocket_, reinterpret_cast<sockaddr *>(&addr),
                       sizeof(addr)) == SOCKET_ERROR)
            {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            if (::listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR)
            {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            ::GUID guidAcceptEx = WSAID_ACCEPTEX;
            ::DWORD bytes = 0;
            if (::WSAIoctl(listenSocket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &guidAcceptEx, sizeof(guidAcceptEx), &pfnAcceptEx_,
                           sizeof(pfnAcceptEx_), &bytes, nullptr,
                           nullptr) == SOCKET_ERROR)
            {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            ::GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
            if (::WSAIoctl(listenSocket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                           &pfnGetAcceptExSockaddrs_, sizeof(pfnGetAcceptExSockaddrs_),
                           &bytes, nullptr, nullptr) == SOCKET_ERROR)
            {
                throw std::system_error(::WSAGetLastError(), std::system_category());
            }

            if (::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket_), iocp_,
                                         0, 0) == nullptr)
            {
                throw std::system_error(static_cast<int>(::GetLastError()),
                                        std::system_category());
            }
            std::println("iocp_context() init net done");
        }
        ~iocp_context() noexcept
        {
            if (listenSocket_ != INVALID_SOCKET)
            {
                ::closesocket(listenSocket_);
            }
            if (iocp_ != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(iocp_);
            }
            ::WSACleanup();
            std::println("~iocp_context() Cleanup net");
        }

        iocp_context(const iocp_context &) = delete;
        iocp_context(iocp_context &&) = delete;
        iocp_context &operator=(const iocp_context &) = delete;
        iocp_context &operator=(iocp_context &&) = delete;

        void run() noexcept
        {
            while (true)
            {
                DWORD bytes = 0;
                ULONG_PTR key = 0;
                OVERLAPPED *overlapped = nullptr;
                ::BOOL ret = ::GetQueuedCompletionStatus(iocp_, &bytes, &key, &overlapped,
                                                         INFINITE);
                if (finish_.load(std::memory_order_relaxed))
                {
                    std::println("finish run function....");
                    break;
                }
                if (FALSE == ret)
                {
                    DWORD err = ::GetLastError();
                    if (overlapped != nullptr)
                        iocp::delete_operation(
                            reinterpret_cast<iocp::IOCPOperationBase *>(overlapped),
                            static_cast<int>(err));

                    if (err == WAIT_TIMEOUT)
                        continue;

                    std::println("GetQueuedCompletionStatus() error: {}", err);
                    continue;
                }

                auto *op = reinterpret_cast<iocp::IOCPOperationBase *>(overlapped);
                switch (op->op_type)
                {
                case io_type::IO_ACCEPT: {
                    auto *o = reinterpret_cast<iocp::AcceptOp *>(op);

                    SOCKET client_socket = o->client_socket;
                    o->client_socket = INVALID_SOCKET; // 解除关联

                    // NOLINTNEXTLINE
                    sockaddr *pLocalAddr = nullptr, *pRemoteAddr = nullptr; // NOLINT
                    // 调用GetAcceptExSockaddrs时使用正确的长度参数
                    pfnGetAcceptExSockaddrs_(o->buffer, 0,
                                             iocp::MIN_ADDR_LENGTH, // 本地地址保留长度
                                             iocp::MIN_ADDR_LENGTH, // 远程地址保留长度
                                             &pLocalAddr, &o->localLen, &pRemoteAddr,
                                             &o->remoteLen);
                    // 将获取的地址复制到结构体中
                    ::memcpy(&o->local_addr, pLocalAddr, o->localLen);
                    ::memcpy(&o->remote_addr, pRemoteAddr, o->remoteLen);

                    // UPDATE_ACCEPT_CONTEXT
                    ::setsockopt(client_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                                 reinterpret_cast<char *>(&listenSocket_),
                                 sizeof(listenSocket_));

                    // client_socket bind iocp_
                    ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket),
                                             iocp_, 0, 0);

                    o->parent_op->complete(o->parent_op);
                }
                case io_type::IO_READ:
                case io_type::IO_WRITE:
                    break;
                default: {
                    std::println("io_type type error!");
                    std::terminate();
                    break;
                }
                }
            }
        }
        auto make_accept() noexcept; // NOLINT

        [[nodiscard]] ::SOCKET listenSocket() const
        {
            return listenSocket_;
        }

        [[nodiscard]] ::LPFN_ACCEPTEX pfnAcceptEx() const
        {
            return pfnAcceptEx_;
        }

        void finish() noexcept
        {
            ::PostQueuedCompletionStatus(iocp_, 0, 0, nullptr);
            this->finish_.store(true, std::memory_order::memory_order_release);
        }

        [[nodiscard]] bool is_stopped() const noexcept // NOLINT
        {
            return finish_.load(std::memory_order_relaxed);
        }

      private:
        ::HANDLE iocp_;
        ::SOCKET listenSocket_;
        ::LPFN_ACCEPTEX pfnAcceptEx_{};
        ::LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_{};
        std::vector<iocp::Connection> connections_;
        // PreConnection;
        std::set<adaptors::PreConnection> preConnections_;
        std::atomic<bool> finish_;

        friend adaptors::PreConnection;
    };

    namespace adaptors
    {
        template <class Sndr, class Rcvr>
        struct operation
        {
            Sndr *sndr;
            Rcvr rcvr;
        };

        template <class Rcvr>
        struct PreAcceptOperation;

        struct PreConnection
        {
            using sender_concept = ::mcs::execution::sender_t;
            using completion_signatures =
                ::mcs::execution::cmplsigs::completion_signatures<
                    mcs::execution::set_value_t(iocp::Connection),
                    mcs::execution::set_error_t(std::exception_ptr)>;

            using indices_for = std::index_sequence_for<>;

            iocp::AcceptOp *acceptOp{}; // NOLINT
            iocp_context *io_ctx{};     // NOLINT

            [[nodiscard]] auto constexpr get_env() const noexcept // NOLINT
            {
                return ::mcs::execution::empty_env{};
            }

            template <::mcs::execution::receiver Rcvr>
            constexpr auto connect(Rcvr rcvr) noexcept -> PreAcceptOperation<Rcvr>
            {
                return PreAcceptOperation<Rcvr>(*this, std::move(rcvr));
            };
        };

        static_assert(::mcs::execution::sender<PreConnection>);
        template <class Rcvr>
        struct PreAcceptOperation : iocp::OperationBase
        {
            using operation_state_concept = ::mcs::execution::operation_state_t;

            ~PreAcceptOperation() noexcept = default;
            PreAcceptOperation(PreAcceptOperation &&) = delete;
            PreAcceptOperation(const PreAcceptOperation &) = delete;
            PreAcceptOperation &operator=(PreAcceptOperation &&) = delete;
            PreAcceptOperation &operator=(const PreAcceptOperation &) = delete;

            PreAcceptOperation(PreConnection &c, Rcvr &&r) noexcept
                : OperationBase{.complete = &PreAcceptOperation::invoke,
                                .error = &PreAcceptOperation::invoke_error},
                  conn(c), rcvr(std::move(r))
            {
                conn.acceptOp->parent_op = this;
            }

            void start() & noexcept
            {
                auto listenSocket_ = conn.io_ctx->listenSocket();
                auto *op = conn.acceptOp;
                auto pfnAccept = conn.io_ctx->pfnAcceptEx();

                std::println("[LISTEN: {} ] Posting AcceptEx...", listenSocket_);
                ::DWORD bytes_received = 0;
                ::BOOL result = pfnAccept(listenSocket_, op->client_socket, op->buffer, 0,
                                          iocp::MIN_ADDR_LENGTH, iocp::MIN_ADDR_LENGTH,
                                          &bytes_received, op->getOVERLAPPED());

                if ((result == FALSE) && ::WSAGetLastError() != ERROR_IO_PENDING)
                {
                    std::println("AcceptEx failed: {}", ::WSAGetLastError());
                    this->error_complete(::WSAGetLastError());
                }
            }
            // 完成
            void complete() noexcept
            {
                iocp::Connection c(conn.acceptOp);

                ::mcs::execution::set_value(std::move(rcvr), std::move(c));
            }
            void error_complete(int err) noexcept // NOLINT
            {
                try
                {
                    throw std::runtime_error{std::string("iocp error code: ") +
                                             std::to_string(err)};
                }
                catch (...)
                {
                    // std::rethrow_exception(std::current_exception());
                    ::mcs::execution::set_error(std::move(rcvr),
                                                std::current_exception());
                }
                // ::mcs::execution::set_error(std::move(rcvr), err);
            }

            static void invoke(OperationBase *self) noexcept
            {
                auto *derived = static_cast<PreAcceptOperation *>(self);
                derived->complete();
            }
            static void invoke_error(OperationBase *self, int code) noexcept // NOLINT
            {
                auto *derived = static_cast<PreAcceptOperation *>(self);
                derived->error_complete(code);
            }

            PreConnection conn; // NOLINT
            Rcvr rcvr;          // NOLINT
        };
    }; // namespace adaptors

    // NOTE: start() 应该注册 监听连接 +  PreConnection的complete延迟：先注册到监听队列中
    auto iocp_context::make_accept() noexcept // NOLINT
    {
        auto *op = new (std::nothrow) iocp::AcceptOp(listenSocket_);
        if (op == nullptr)
        {
            std::println("new AcceptOp() failed");
            std::terminate();
        }
        return adaptors::PreConnection{.acceptOp = op, .io_ctx = this};
    }

}; // namespace mcs::net::io

#endif