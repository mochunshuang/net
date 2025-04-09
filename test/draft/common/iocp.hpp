#pragma once

#include <cassert>
#include <cmath>
#include <coroutine>
#include <csignal>
#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <iostream>
#include <latch>
#include <stdexcept>
#include <string>
#include <print>
#include <type_traits>

#if defined(_MSC_VER)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h> // 包含 InetNtop 函数的头文件

#pragma comment(lib, "ws2_32.lib")

#include "./SafeHeapManager.hpp"

enum io_type : std::int8_t
{
    IO_UNKNOWN,
    IO_ACCEPT,
    IO_READ,
    IO_WRITE,
    IO_DISCONNECT,
};

using IocpAsyncResult = std::pair<DWORD, DWORD>;

struct IocpAcceptor
{
    ::SOCKET listen_socket;         // NOLINT
    ::SOCKET accept_socket;         // NOLINT
    ::sockaddr_storage local_addr;  // NOLINT
    ::sockaddr_storage remote_addr; // NOLINT

    IocpAcceptor() noexcept
        : listen_socket{INVALID_SOCKET}, accept_socket{INVALID_SOCKET}, local_addr{},
          remote_addr{}
    {
    }
    IocpAcceptor(const IocpAcceptor &) = delete;
    IocpAcceptor &operator=(const IocpAcceptor &) = delete;
    IocpAcceptor(IocpAcceptor &&) = delete;
    IocpAcceptor &operator=(IocpAcceptor &&) = delete;
    ~IocpAcceptor() = default;
};

struct IocpCompletionData
{
    std::coroutine_handle<> continuation; // 外部协程绑定至此
    ::DWORD bytes_transferred = 0;
    ::DWORD error_code = ERROR_SUCCESS;
};

struct IocpOperationBase : public OVERLAPPED
{
    // 支持 IPv4 和 IPv6
    static constexpr size_t MAX_ADDR_LENGTH = sizeof(::sockaddr_storage) + 16; // NOLINT
    static constexpr size_t MIN_ADDR_LENGTH = sizeof(::sockaddr_in) + 16;      // NOLINT

    IocpOperationBase() noexcept : OVERLAPPED{}, op_type{IO_UNKNOWN}, completion_data{}
    {
        assert(this->Internal == 0);
    }
    io_type op_type;                    // NOLINT
    IocpCompletionData completion_data; // NOLINT
};

struct IocpAcceptOp : public IocpOperationBase
{
    IocpAcceptOp() noexcept
    {
        op_type = io_type::IO_ACCEPT;
    }
    char addressBuffer[4096]{}; // NOLINT
    IocpAcceptor acceptor{};    // NOLINT
};

struct IocpReadOp : public IocpOperationBase
{
    WSABUF buffer;                // NOLINT
    const IocpAcceptor *acceptor; // NOLINT

    explicit IocpReadOp(const IocpAcceptor *s, ::CHAR *data, ::ULONG size)
        : buffer{}, acceptor(s)
    {
        op_type = io_type::IO_READ;
        buffer.buf = data;
        buffer.len = size;
    }
};

struct IocpAwaitable
{
    IocpOperationBase *op; // NOLINT
    bool ready;            // NOLINT
    explicit IocpAwaitable(IocpOperationBase *p, bool ready = false) noexcept
        : op(p), ready(ready)
    {
    }

    bool await_ready() noexcept // NOLINT
    {
        return ready;
    }

    void await_suspend(std::coroutine_handle<> h) const noexcept // NOLINT
    {
        op->completion_data.continuation = h; // 外部协程绑定
    }

    IocpAsyncResult await_resume() noexcept // NOLINT
    {
        return {op->completion_data.bytes_transferred, op->completion_data.error_code};
    }
};

struct endpoint
{
    uint32_t addr{};
    uint16_t port{8080}; // NOLINT
    uint8_t protocol{IPPROTO_TCP};
    uint8_t family{AF_INET};
    uint8_t type{SOCK_STREAM};
};

namespace WAIT
{
    static constexpr std::int8_t NUM_THREAD = 3; // NOLINT
    static std::latch &wait_net_done() noexcept  // NOLINT
    {
        static std::latch work_done{NUM_THREAD};
        return work_done;
    }
    static auto &registration_list() noexcept // NOLINT
    {
        static std::forward_list<HANDLE> list;
        return list;
    }
    static void register_wait_done(HANDLE iocp) noexcept // NOLINT
    {
        registration_list().push_front(iocp);
    }
    // 定义一个原子标志位，用于指示是否退出循环
    inline static std::atomic<bool> &get_exit_flag() noexcept // NOLINT
    {
        static std::atomic<bool> exit_flag(false);
        return exit_flag;
    }

    // 信号处理函数
    inline static void signal_handler(int signal) noexcept // NOLINT
    {
        if (signal == SIGINT) // 捕获Ctrl+C信号
        {
            std::println("\nReceived SIGINT (Ctrl+C). Exiting...\n");
            get_exit_flag().store(true, std::memory_order_release); // 设置标志位为true
            for (auto *iocp : registration_list())
                ::PostQueuedCompletionStatus(iocp, 0, 0, nullptr);
            WAIT::wait_net_done().wait();
        }
    }
}; // namespace WAIT

struct iocp_context
{
  private:
    HANDLE iocp_{};
    SOCKET listenSocket_{};
    LPFN_ACCEPTEX pfnAcceptEx_{};
    LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_{};
    endpoint config_{};

    void clear()
    {
        if (listenSocket_ != INVALID_SOCKET)
        {
            ::closesocket(listenSocket_);
            listenSocket_ = INVALID_SOCKET;
        }
        if (iocp_ != nullptr)
        {
            ::CloseHandle(iocp_);
            iocp_ = nullptr;
        }
        ::WSACleanup();
    }

  public:
    static auto applyAddressString(const ::sockaddr_storage &addr) -> std::string
    {
        char ipStr[INET6_ADDRSTRLEN]; // NOLINT
        uint16_t port = 0;

        if (addr.ss_family == AF_INET)
        {
            auto *sin = reinterpret_cast<const sockaddr_in *>(&addr); // NOLINT
            inet_ntop(AF_INET, &sin->sin_addr, ipStr, sizeof(ipStr));
            port = ntohs(sin->sin_port);
        }
        else if (addr.ss_family == AF_INET6)
        {
            auto *sin6 = reinterpret_cast<const sockaddr_in6 *>(&addr); // NOLINT
            inet_ntop(AF_INET6, &sin6->sin6_addr, ipStr, sizeof(ipStr));
            port = ntohs(sin6->sin6_port);
        }
        else
        {
            return "Unknown Address Family";
        }
        return std::string(ipStr) + ":" + std::to_string(port);
    };

    iocp_context(const iocp_context &) = delete;
    iocp_context(iocp_context &&) = delete;
    iocp_context &operator=(const iocp_context &) = delete;
    iocp_context &operator=(iocp_context &&) = delete;
    explicit iocp_context(endpoint config) noexcept : config_(config)
    {
        std::cerr << "iocp_context(const endpoint &config)" << '\n';
        try
        {
            if (WSADATA wsaData{}; ::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
                throw std::runtime_error("WSAStartup error");
            if (iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
                iocp_ == nullptr)
                throw std::runtime_error("CreateIoCompletionPort error");
            if (listenSocket_ = // NOLINTNEXTLINE
                ::WSASocketW(config_.family, config_.type, 0, nullptr, 0,
                             WSA_FLAG_OVERLAPPED);
                listenSocket_ == INVALID_SOCKET)
                throw std::runtime_error("WSASocket error");

            // Set IO to NBIO
            if (u_long u1 = 1; ::ioctlsocket(listenSocket_, FIONBIO, &u1) == SOCKET_ERROR)
                throw std::runtime_error("ioctlsocket error");
            // m_listenFd close send/reciver
            char size = 0;
            ::setsockopt(listenSocket_, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
            ::setsockopt(listenSocket_, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
        }
        catch (const std::runtime_error &e)
        {
            clear();
            std::cerr << "Caught exception in anotherFunction: " << e.what() << '\n';
            std::abort();
        }
    }
    ~iocp_context() noexcept
    {
        clear();
    }
    void bind() noexcept
    {
        try
        {
            sockaddr_in address{};
            address.sin_family = config_.family;
            address.sin_port = ::htons(config_.port);
            address.sin_addr.s_addr = config_.addr;
            if (::bind(listenSocket_,
                       reinterpret_cast<const sockaddr *>(&address), // NOLINT
                       sizeof(address)) != 0)
                throw std::runtime_error("socket bind error");
        }
        catch (const std::runtime_error &e)
        {
            clear();
            std::cerr << "Caught exception in anotherFunction: " << e.what() << '\n';
            std::abort();
        }
    }
    void listen() noexcept
    {
        try
        {
            if (::listen(listenSocket_, SOMAXCONN) != 0)
                throw std::runtime_error("socket listen error");

            // bind listenSocket_ and  iocp_
            if (::CreateIoCompletionPort(
                    reinterpret_cast<HANDLE>(listenSocket_), // NOLINT
                    iocp_, 0, 0) == nullptr)
                throw std::runtime_error("bind socket and completePort error");

            GUID GuidAcceptEx = WSAID_ACCEPTEX;
            DWORD dwBytes = 0;
            if (SOCKET_ERROR ==
                ::WSAIoctl(listenSocket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &GuidAcceptEx, sizeof(GuidAcceptEx),
                           (void *)&pfnAcceptEx_, // NOLINT
                           sizeof(pfnAcceptEx_), &dwBytes, nullptr, nullptr))
                throw std::runtime_error("set m_pfnAcceptEx error");

            GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
            if (SOCKET_ERROR ==
                ::WSAIoctl(listenSocket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                           static_cast<void *>(&pfnGetAcceptExSockaddrs_),
                           sizeof(pfnGetAcceptExSockaddrs_), &dwBytes, nullptr, nullptr))
                throw std::runtime_error("set m_pfnGetAcceptExSockaddrs error");
        }
        catch (const std::runtime_error &e)
        {
            clear();
            std::cerr << "Caught exception in anotherFunction: " << e.what() << '\n';
            std::abort();
        }
    }

    static auto &accept_operation_pool() noexcept // NOLINT
    {
        static SafeHeapManager<IocpAcceptOp> pool;
        return pool;
    }
    static auto &read_operation_pool() noexcept // NOLINT
    {
        static SafeHeapManager<IocpReadOp> pool;
        return pool;
    }

    // NOTE: 没有释放 IocpAcceptOp
    static void deallocate_operation(IocpOperationBase *io_op) noexcept // NOLINT
    {
        if (io_op->op_type == IO_READ)
        {
            read_operation_pool().deallocate(static_cast<IocpReadOp *>(io_op)); // NOLINT
        }
    }

    static void handle_accept(IocpOperationBase *io_op) noexcept // NOLINT
    {
        if (io_op->completion_data.continuation)
            io_op->completion_data.continuation.resume();
    };
    auto getAsyncAcceptOp() noexcept
    {
        auto &pool = iocp_context::accept_operation_pool();
        IocpAcceptOp *op = pool.allocate();
        op->acceptor.listen_socket = listenSocket_;
        try
        {
            auto accept_socket = ::WSASocketW(config_.family, config_.type, 0, nullptr, 0,
                                              WSA_FLAG_OVERLAPPED);
            if (accept_socket == INVALID_SOCKET)
                throw std::runtime_error("WSASocket error");
            op->acceptor.accept_socket = accept_socket;
        }
        catch (const std::runtime_error &e)
        {
            clear();
            std::cerr << "Caught exception in anotherFunction: " << e.what() << '\n';
            std::abort();
        }
        return op;
    }

    // 在main 线程启动
    void run() noexcept
    {
        WAIT::register_wait_done(iocp_);

        std::println("TcpListenServer::run start");

        auto *op = getAsyncAcceptOp();
        if (FALSE == doPostAsyncAccept(op))
        {
            std::println("getAsyncAcceptOp ERROR");
            std::abort();
        };

        ::DWORD bytesTransferred{};
        ::ULONG_PTR completionKey{};
        IocpOperationBase *io_op{};
        static_assert(std::is_base_of_v<OVERLAPPED, IocpOperationBase>);
        ::BOOL ret{};
        while (true)
        {
            ret = ::GetQueuedCompletionStatus(iocp_, &bytesTransferred, &completionKey,
                                              reinterpret_cast<OVERLAPPED **>(&io_op),
                                              INFINITE); // NOLINT
            std::println("GetQueuedCompletionStatus OK");
            if (WAIT::get_exit_flag().load(std::memory_order_relaxed))
            {
                WAIT::wait_net_done().count_down();
                break;
            }

            if (FALSE == ret)
            {
                auto Error = ::GetLastError();
                if (Error == WAIT_TIMEOUT)
                {
                    std::cout << "GetQueuedCompletionStatus Error: WAIT_TIMEOUT" << '\n';
                    continue;
                }
                if (Error == ERROR_NETNAME_DELETED)
                {
                    std::cerr
                        << "GetQueuedCompletionStatus Error: ERROR_NETNAME_DELETED\n";
                    // TODO(mcs): 不严谨
                    deallocate_operation(io_op);
                    continue;
                }
                std::cerr << "GetQueuedCompletionStatus error not handle\n";
                std::abort();
                break;
            }
            if (io_op->op_type == IO_ACCEPT)
            {
                std::println("handle_accept");
                handle_accept(io_op);
                continue;
            }
            std::cerr << "io_op_type error\n";
            break;
        }
        std::println("TcpListenServer::run end. exit_flag: {}",
                     WAIT::get_exit_flag().load(std::memory_order_relaxed));
    }

    BOOL doPostAsyncAccept(IocpAcceptOp *op) noexcept
    {
        assert(op->acceptor.listen_socket != INVALID_SOCKET);
        assert(listenSocket_ != INVALID_SOCKET);
        // 投递 AcceptEx 请求
        // NOTE: 缓冲区大小是否满足？是否刚刚好
        DWORD bytes_received = 0;
        BOOL result = pfnAcceptEx_(listenSocket_,              // 监听套接字
                                   op->acceptor.accept_socket, // 接受套接字
                                   op->addressBuffer,          // 接收缓冲区
                                   0, // 接收缓冲区中用于存储本地地址和远程地址的空间大小
                                   IocpAcceptOp::MIN_ADDR_LENGTH,      // 本地地址长度
                                   IocpAcceptOp::MIN_ADDR_LENGTH,      // 远程地址长度
                                   &bytes_received,                    // 接收的字节数
                                   reinterpret_cast<OVERLAPPED *>(&op) // NOLINT
        );

        if (FALSE == result)
        {
            int error = ::WSAGetLastError();
            if (error != ERROR_IO_PENDING)
            {
                std::cerr << "AcceptEx failed: " << error << '\n';
                return FALSE;
            }
        }

        return TRUE;
    }

    static BOOL postAsyncRecv(IocpReadOp *op) noexcept
    {

        assert(op->completion_data.continuation != nullptr);

        ::DWORD flags{};
        // 投递异步接收操作
        int result = ::WSARecv(op->acceptor->accept_socket, // 目标 socket
                               &op->buffer,                 // 接收缓冲区
                               1,                           // 缓冲区数量
                               nullptr, // 接收的字节数（异步操作时不使用）
                               &flags,  // 标志位
                               static_cast<OVERLAPPED *>(op), // 重叠结构
                               nullptr                        // 完成例程（IOCP 中不使用）
        );

        if (result == SOCKET_ERROR)
        {
            int error = WSAGetLastError();
            if (error != WSA_IO_PENDING)
            {
                std::cerr << "WSARecv failed: " << error << '\n';
                return FALSE;
            }
        }
        return TRUE;
    }

    auto asyncAccept() noexcept
    {
        auto *op = getAsyncAcceptOp();
        if (FALSE == doPostAsyncAccept(op))
        {
            op->completion_data.error_code = ::WSAGetLastError();
            return IocpAwaitable{op, true};
        }
        return IocpAwaitable{op};
    }
};

struct socket_acceptor
{
    endpoint e;
    iocp_context &ctx;

    // socket_acceptor(endpoint e, iocp_context ctx) : e(e), ctx(e)
    // {

    // }
};

//========
struct async_accept_t
{
    auto operator()(iocp_context &ctx) const noexcept
    {
        //
        return ctx.asyncAccept();
    }
};
struct async_connect_t
{
};
struct async_send_t
{
};
struct async_send_to_t
{
};
struct async_receive_t
{
};
struct async_receive_from_t
{
};
constexpr inline async_accept_t async_accept{};             // NOLINT
constexpr inline async_connect_t async_connect{};           // NOLINT
constexpr inline async_send_t async_send{};                 // NOLINT
constexpr inline async_send_to_t async_send_to{};           // NOLINT
constexpr inline async_receive_t async_receive{};           // NOLINT
constexpr inline async_receive_from_t async_receive_from{}; // NOLINT

#endif