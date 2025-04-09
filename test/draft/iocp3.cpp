#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <string>
#include <system_error>
#include <vector>

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h>

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
    io_type type;
    SOCKET socket;
    IocpOperationBase(io_type t, SOCKET s) noexcept : OVERLAPPED{}, type(t), socket(s) {}
};

struct IocpAcceptOp : public IocpOperationBase
{
    static constexpr auto MIN_ADDR_LENGTH = sizeof(sockaddr_in) + 16;
    SOCKET client_socket;
    char buffer[2 * MIN_ADDR_LENGTH];

    explicit IocpAcceptOp(SOCKET listen_socket) noexcept
        : IocpOperationBase(IO_ACCEPT, listen_socket),
          client_socket(WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0,
                                   WSA_FLAG_OVERLAPPED)),
          buffer{}
    {
        if (client_socket == INVALID_SOCKET)
        {
            std::cerr << "WSASocket failed: " << WSAGetLastError() << '\n';
            std::abort();
        }
    }
    ~IocpAcceptOp() noexcept
    {
        if (client_socket != INVALID_SOCKET)
            closesocket(client_socket);
    }
    IocpAcceptOp(IocpAcceptOp &&) = delete;
    IocpAcceptOp(const IocpAcceptOp &) = delete;
    IocpAcceptOp &operator=(IocpAcceptOp &&) = delete;
    IocpAcceptOp &operator=(const IocpAcceptOp &) = delete;
};

struct IocpReadOp : public IocpOperationBase
{
    static constexpr std::size_t BUFF_SIZE = 4096;
    char buffer[BUFF_SIZE];
    WSABUF wsabuf;
    explicit IocpReadOp(SOCKET s) noexcept
        : IocpOperationBase(IO_READ, s), buffer{}, wsabuf{}
    {
        wsabuf.buf = buffer;
        wsabuf.len = BUFF_SIZE;
    }
};

struct IocpWriteOp : public IocpOperationBase
{
    WSABUF wsabuf;
    std::vector<char> buffer;

    IocpWriteOp(SOCKET s, const char *data, size_t len) noexcept
        : IocpOperationBase(IO_WRITE, s), buffer(data, data + len)
    {
        wsabuf.buf = buffer.data();
        wsabuf.len = static_cast<ULONG>(len);
    }
};

class ThreadPool
{
  public:
    explicit ThreadPool(size_t threads) : stop(false)
    {
        for (size_t i = 0; i < threads; ++i)
        {
            workers.emplace_back([this] {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template <class F>
    void submit(F &&f)
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop)
                throw std::runtime_error("submit on stopped ThreadPool");
            tasks.emplace(std::forward<F>(f));
        }
        condition.notify_one();
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            if (worker.joinable())
                worker.join();
    }

  private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

struct iocp_context
{
  private:
    HANDLE iocp_;
    SOCKET listen_socket_;
    LPFN_ACCEPTEX pfnAcceptEx_;
    LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_;
    ThreadPool thread_pool;

    void post_accept()
    {
        auto op = new IocpAcceptOp(listen_socket_);
        DWORD bytes_received = 0;
        BOOL result =
            pfnAcceptEx_(listen_socket_, op->client_socket, op->buffer, 0,
                         IocpAcceptOp::MIN_ADDR_LENGTH, IocpAcceptOp::MIN_ADDR_LENGTH,
                         &bytes_received, static_cast<OVERLAPPED *>(op));

        if (!result && WSAGetLastError() != ERROR_IO_PENDING)
        {
            std::cerr << "AcceptEx failed: " << WSAGetLastError() << '\n';
            delete op;
        }
    }

  public:
    iocp_context()
        : iocp_(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0)),
          listen_socket_(INVALID_SOCKET), thread_pool(std::thread::hardware_concurrency())
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            throw std::system_error(WSAGetLastError(), std::system_category());
        }

        if (iocp_ == nullptr)
        {
            throw std::system_error(GetLastError(), std::system_category());
        }

        listen_socket_ = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0,
                                    WSA_FLAG_OVERLAPPED);
        if (listen_socket_ == INVALID_SOCKET)
        {
            throw std::system_error(WSAGetLastError(), std::system_category());
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(8080);

        if (bind(listen_socket_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) ==
            SOCKET_ERROR)
        {
            throw std::system_error(WSAGetLastError(), std::system_category());
        }

        if (listen(listen_socket_, SOMAXCONN) == SOCKET_ERROR)
        {
            throw std::system_error(WSAGetLastError(), std::system_category());
        }

        GUID guidAcceptEx = WSAID_ACCEPTEX;
        DWORD bytes = 0;
        if (WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidAcceptEx,
                     sizeof(guidAcceptEx), &pfnAcceptEx_, sizeof(pfnAcceptEx_), &bytes,
                     nullptr, nullptr) == SOCKET_ERROR)
        {
            throw std::system_error(WSAGetLastError(), std::system_category());
        }

        GUID guidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
        if (WSAIoctl(listen_socket_, SIO_GET_EXTENSION_FUNCTION_POINTER,
                     &guidGetAcceptExSockaddrs, sizeof(guidGetAcceptExSockaddrs),
                     &pfnGetAcceptExSockaddrs_, sizeof(pfnGetAcceptExSockaddrs_), &bytes,
                     nullptr, nullptr) == SOCKET_ERROR)
        {
            throw std::system_error(WSAGetLastError(), std::system_category());
        }

        if (!CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket_), iocp_, 0,
                                    0))
        {
            throw std::system_error(GetLastError(), std::system_category());
        }

        post_accept();
    }

    ~iocp_context()
    {
        if (listen_socket_ != INVALID_SOCKET)
        {
            closesocket(listen_socket_);
        }
        if (iocp_ != INVALID_HANDLE_VALUE)
        {
            CloseHandle(iocp_);
        }
        WSACleanup();
    }

    void post_write(SOCKET socket, const char *data, size_t len)
    {
        auto op = new IocpWriteOp(socket, data, len);
        DWORD sent = 0;
        if (WSASend(socket, &op->wsabuf, 1, &sent, 0, op, nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                closesocket(socket);
                delete op;
            }
        }
    }

    void post_read(SOCKET socket)
    {
        auto op = new IocpReadOp(socket);
        DWORD flags = 0;
        if (WSARecv(socket, &op->wsabuf, 1, nullptr, &flags, op, nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                closesocket(socket);
                delete op;
            }
        }
    }

    void run()
    {
        while (true)
        {
            DWORD bytes = 0;
            ULONG_PTR key = 0;
            OVERLAPPED *overlapped = nullptr;

            if (!GetQueuedCompletionStatus(iocp_, &bytes, &key, &overlapped, INFINITE))
            {
                DWORD err = GetLastError();
                if (overlapped)
                    delete static_cast<IocpOperationBase *>(overlapped);
                if (err == WAIT_TIMEOUT)
                    continue;
                std::cerr << "Completion error: " << err << '\n';
                continue;
            }

            auto op = static_cast<IocpOperationBase *>(overlapped);
            switch (op->type)
            {
            case IO_ACCEPT: {
                auto accept_op = static_cast<IocpAcceptOp *>(op);
                SOCKET client = accept_op->client_socket;
                accept_op->client_socket = INVALID_SOCKET;

                setsockopt(client, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                           reinterpret_cast<char *>(&listen_socket_), sizeof(SOCKET));
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(client), iocp_, 0, 0);
                post_accept();
                post_read(client);
                delete accept_op;
                break;
            }

            case IO_READ: {
                auto read_op = static_cast<IocpReadOp *>(op);
                if (bytes == 0)
                {
                    closesocket(read_op->socket);
                    delete read_op;
                    break;
                }

                thread_pool.submit([this, read_op, bytes] {
                    std::string response = "HTTP/1.1 200 OK\r\n"
                                           "Content-Type: text/plain\r\n"
                                           "Content-Length: 12\r\n\r\n"
                                           "Hello World!";
                    post_write(read_op->socket, response.data(), response.size());
                    delete read_op;
                });
                break;
            }

            case IO_WRITE: {
                auto write_op = static_cast<IocpWriteOp *>(op);
                thread_pool.submit([write_op] {
                    closesocket(write_op->socket);
                    delete write_op;
                });
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
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "Unknown exception occurred\n";
    }
    return 0;
}