
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <string>
#include <system_error>
#include <vector>

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h> // 包含 InetNtop 函数的头文件

#pragma comment(lib, "ws2_32.lib")
enum io_type : std::int8_t
{
    IO_UNKNOWN,
    IO_ACCEPT,
    IO_READ,
    IO_WRITE,
    IO_DISCONNECT,
};

struct IocpOperationBase : public OVERLAPPED
{
    io_type type;           // NOLINT
    ::SOCKET listen_socket; // NOLINT
    IocpOperationBase(io_type t, ::SOCKET s) noexcept
        : OVERLAPPED{}, type(t), listen_socket(s)
    {
    }
};

struct IocpAcceptOp : public IocpOperationBase
{
    static constexpr auto MIN_ADDR_LENGTH = sizeof(sockaddr_in) + 16; // NOLINT
    ::SOCKET client_socket;                                           // NOLINT
    char buffer[2 * MIN_ADDR_LENGTH];                                 // NOLINT

    explicit IocpAcceptOp(::SOCKET listen_socket) noexcept
        : IocpOperationBase(IO_ACCEPT, listen_socket),
          client_socket(::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0,
                                     WSA_FLAG_OVERLAPPED)),
          buffer{}
    {
        if (client_socket == INVALID_SOCKET)
        {
            std::cerr << "WSASocket failed: " << ::WSAGetLastError() << '\n';
            std::abort();
        }
    }
    ~IocpAcceptOp() noexcept
    {
        if (client_socket != INVALID_SOCKET)
            ::closesocket(client_socket);
    }

    IocpAcceptOp(IocpAcceptOp &&) = delete;
    IocpAcceptOp(const IocpAcceptOp &) = delete;
    IocpAcceptOp &operator=(IocpAcceptOp &&) = delete;
    IocpAcceptOp &operator=(const IocpAcceptOp &) = delete;
};

struct IocpReadOp : public IocpOperationBase
{
    static constexpr std::size_t BUFF_SIZE = 4096; // NOLINT
    char buffer[BUFF_SIZE];                        // NOLINT
    ::WSABUF wsabuf;                               // NOLINT
    explicit IocpReadOp(::SOCKET listen_socket) noexcept
        : IocpOperationBase(IO_READ, listen_socket), buffer{}, wsabuf{}
    {
        wsabuf.buf = buffer;
        wsabuf.len = BUFF_SIZE;
    }
};

struct IocpWriteOp : public IocpOperationBase
{
    ::WSABUF wsabuf{};        // NOLINT //TODO buffer 可以改进
    std::vector<char> buffer; // 内部持有数据副本 // NOLINT

    IocpWriteOp(::SOCKET listen_socket, const char *data, size_t len) noexcept
        : IocpOperationBase(IO_WRITE, listen_socket), buffer(data, data + len)
    {
        wsabuf.buf = buffer.data();
        wsabuf.len = static_cast<ULONG>(len);
    }
};

struct iocp_context
{
  private:
    HANDLE iocp_;
    SOCKET listenSocket_;
    LPFN_ACCEPTEX pfnAcceptEx_{};
    LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_{};

  public:
    iocp_context(iocp_context &&) = delete;
    iocp_context(const iocp_context &) = delete;
    iocp_context &operator=(iocp_context &&) = delete;
    iocp_context &operator=(const iocp_context &) = delete;
    iocp_context() : iocp_{INVALID_HANDLE_VALUE}, listenSocket_(INVALID_SOCKET)
    {

        // NOTE: SocKet的API WSAStartup后才能使用
        //  初始化Winsock. 如果成功， WSAStartup 函数返回零。
        if (WSADATA wsaData; 0 != ::WSAStartup(MAKEWORD(2, 2), &wsaData))
            throw std::system_error(::WSAGetLastError(), std::system_category());

        // 完成端口
        iocp_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, {}, {});
        if (iocp_ == nullptr)
            throw std::system_error(static_cast<int>(::GetLastError()),
                                    std::system_category());

        // 监听socket
        listenSocket_ = ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0,
                                     WSA_FLAG_OVERLAPPED);
        if (listenSocket_ == INVALID_SOCKET)
            throw std::system_error(::WSAGetLastError(), std::system_category());

        // 绑定和监听
        sockaddr_in addr = {};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = ::htons(8080);
        // 如果未发生错误， 绑定 将返回零。 否则，它将返回SOCKET_ERROR，并且可以通过调用
        // WSAGetLastError 来检索特定的错误代码。
        if (::bind(listenSocket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) ==
            SOCKET_ERROR)
        {
            throw std::system_error(::WSAGetLastError(), std::system_category());
        }
        if (::listen(listenSocket_, SOMAXCONN) == SOCKET_ERROR)
        {
            throw std::system_error(::WSAGetLastError(), std::system_category());
        }

        // 加载AcceptEx函数
        GUID guidAcceptEx = WSAID_ACCEPTEX;
        if (DWORD bytes = 0;
            ::WSAIoctl(listenSocket_, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx,
                       sizeof(guidAcceptEx), &pfnAcceptEx_, sizeof(pfnAcceptEx_), &bytes,
                       nullptr, nullptr) == SOCKET_ERROR)
            throw std::system_error(::WSAGetLastError(), std::system_category());

        GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
        if (DWORD bytes = 0;
            SOCKET_ERROR ==
            ::WSAIoctl(listenSocket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
                       &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                       &pfnGetAcceptExSockaddrs_, sizeof(pfnGetAcceptExSockaddrs_),
                       &bytes, nullptr, nullptr))
            throw std::system_error(::WSAGetLastError(), std::system_category());

        // 将监听socket关联到完成端口
        if (nullptr == ::CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSocket_),
                                                iocp_, {}, {}))
            throw std::system_error(static_cast<int>(::GetLastError()),
                                    std::system_category());
    }

    ~iocp_context()
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
    }

    void post_accept()
    {
        auto op = new IocpAcceptOp(listenSocket_);
        if (op->client_socket == INVALID_SOCKET)
        {
            delete op;
            return;
        }

        DWORD bytes_received = 0;
        BOOL result =
            pfnAcceptEx_(listenSocket_, op->client_socket, op->buffer,
                         0, // 不接收数据
                         IocpAcceptOp::MIN_ADDR_LENGTH, IocpAcceptOp::MIN_ADDR_LENGTH,
                         &bytes_received, static_cast<OVERLAPPED *>(op));

        DWORD last_error = ::WSAGetLastError();
        if (result == FALSE && last_error != ERROR_IO_PENDING)
        {
            std::cerr << "AcceptEx failed: " << last_error << '\n';
            delete op;
        }
        else
        {
            std::cout << "AcceptEx posted successfully" << '\n';
        }
    }

    void run()
    {
        post_accept(); // 初始投递Accept

        while (true)
        {
            DWORD bytes_transferred = 0;
            ULONG_PTR completion_key = 0;
            OVERLAPPED *overlapped = nullptr;

            BOOL ok = ::GetQueuedCompletionStatus(iocp_, &bytes_transferred,
                                                  &completion_key, &overlapped, INFINITE);
            DWORD last_error = (ok == TRUE) ? ERROR_SUCCESS : ::GetLastError();

            if (overlapped == nullptr)
            {
                std::cerr << "GetQueuedCompletionStatus error: " << last_error << '\n';
                continue;
            }

            auto *op = static_cast<IocpOperationBase *>(overlapped);
            std::cout << "Processing operation type: " << static_cast<int>(op->type)
                      << ", socket: " << op->listen_socket
                      << ", bytes: " << bytes_transferred << ", error: " << last_error
                      << '\n';

            if (last_error != ERROR_SUCCESS && last_error != ERROR_NETNAME_DELETED)
            {
                std::cerr << "IO operation failed: " << last_error << '\n';
                ::closesocket(op->listen_socket);
                delete op;
                continue;
            }

            switch (op->type)
            {
            case IO_ACCEPT: {
                auto *accept_op = static_cast<IocpAcceptOp *>(op);
                SOCKET client_socket = accept_op->client_socket;
                accept_op->client_socket = INVALID_SOCKET; // 解除关联

                // UPDATE_ACCEPT_CONTEXT
                ::setsockopt(client_socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                             reinterpret_cast<char *>(&listenSocket_),
                             sizeof(listenSocket_));

                // 注册新socket
                ::CreateIoCompletionPort((HANDLE)client_socket, iocp_, 0, 0);

                // 投递新Accept
                post_accept();

                // 投递读操作
                auto *read_op = new IocpReadOp(client_socket);
                DWORD flags = 0;
                if (::WSARecv(client_socket, &read_op->wsabuf, 1, nullptr, &flags,
                              read_op, nullptr) == SOCKET_ERROR)
                {
                    if (::WSAGetLastError() != WSA_IO_PENDING)
                    {
                        ::closesocket(client_socket);
                        delete read_op;
                    }
                }
                delete accept_op;
                break;
            }
                // 修改读操作处理逻辑
            case IO_READ: {
                if (bytes_transferred == 0 || last_error == ERROR_NETNAME_DELETED)
                {
                    std::cout << "Connection closed by client, socket: "
                              << op->listen_socket << '\n';
                    ::closesocket(op->listen_socket);
                    delete op;
                    break;
                }

                auto *read_op = static_cast<IocpReadOp *>(op);
                std::cout << "Received " << bytes_transferred
                          << " bytes from socket: " << read_op->listen_socket << '\n';

                std::thread([read_op]() {
                    try
                    {
                        // 确保响应包含完整的HTTP头
                        // 构造响应
                        std::string responseBody = "Hello World!";
                        std::string response = "HTTP/1.1 200 OK\r\n"
                                               "Content-Type: text/plain\r\n"
                                               "Content-Length: " +
                                               std::to_string(responseBody.length()) +
                                               "\r\n"
                                               "\r\n" // 头与正文的空行
                                               + responseBody;
                        auto *write_op = new IocpWriteOp(
                            read_op->listen_socket, response.data(), response.size());

                        DWORD sent = 0;
                        if (::WSASend(write_op->listen_socket, &write_op->wsabuf, 1,
                                      &sent, 0, write_op, nullptr) == SOCKET_ERROR)
                        {
                            int err = ::WSAGetLastError();
                            if (err != WSA_IO_PENDING)
                            {
                                std::cerr << "WSASend failed: " << err
                                          << ", socket: " << write_op->listen_socket
                                          << '\n';
                                ::closesocket(write_op->listen_socket);
                                delete write_op;
                            }
                        }
                        else
                        {
                            std::cout << "WSASend initiated successfully, socket: "
                                      << write_op->listen_socket << '\n';
                        }
                    }
                    catch (...)
                    {
                        throw;
                    }
                    delete read_op; // 确保在最后才释放读操作
                }).detach();
                break;
            }

            case IO_WRITE: {
                auto *write_op = static_cast<IocpWriteOp *>(op);
                std::thread([write_op]() {
                    std::cout << "Closing socket after write: " << write_op->listen_socket
                              << '\n';
                    ::closesocket(write_op->listen_socket);
                    delete write_op;
                }).detach();
                break;
            }
            default:
                delete op;
                break;
            }
        }
    }
};

int main()
{
    try
    {
        iocp_context ctx;
        ctx.run();
    }
    catch (...)
    {
        std::cout << "error\n";
    }
    return 0;
}
