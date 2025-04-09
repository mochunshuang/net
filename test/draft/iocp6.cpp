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
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// 日志系统
std::mutex log_mutex;

#define LOG(msg)                                                                   \
    do                                                                             \
    {                                                                              \
        std::lock_guard<std::mutex> log_lock(log_mutex);                           \
        auto now = std::chrono::system_clock::now();                               \
        auto now_time = std::chrono::system_clock::to_time_t(now);                 \
        std::cout << "[" << std::put_time(std::localtime(&now_time), "%T") << "][" \
                  << std::this_thread::get_id() << "] " << msg << std::endl;       \
    } while (0)

enum io_type : std::int8_t
{
    IO_UNKNOWN,
    IO_ACCEPT,
    IO_READ,
    IO_WRITE,
    IO_DISCONNECT,
};

struct IocpOperationBase : OVERLAPPED
{
    io_type type;
    SOCKET socket;
    IocpOperationBase(io_type t, SOCKET s) noexcept : OVERLAPPED{}, type(t), socket(s) {}
};

struct Session
{
    SOCKET socket;
    std::vector<char> read_buffer;
    std::vector<char> write_buffer;
    size_t write_offset = 0;
    bool keep_alive = false;
    bool headers_parsed = false;

    explicit Session(SOCKET s) : socket(s) {}
};

struct IocpAcceptOp : IocpOperationBase
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
            LOG("WSASocket failed: " << WSAGetLastError());
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

struct IocpReadOp : IocpOperationBase
{
    static constexpr std::size_t BUFF_SIZE = 8;
    char buffer[BUFF_SIZE];
    WSABUF wsabuf;
    Session *session;

    explicit IocpReadOp(SOCKET s, Session *sess) noexcept
        : IocpOperationBase(IO_READ, s), buffer{}, wsabuf{}, session(sess)
    {
        wsabuf.buf = buffer;
        wsabuf.len = BUFF_SIZE;
    }
};

struct IocpWriteOp : IocpOperationBase
{
    WSABUF wsabuf;
    std::vector<char> buffer;
    bool closeAfterWrite;

    IocpWriteOp(SOCKET s, const char *data, size_t len, bool closeAfter) noexcept
        : IocpOperationBase(IO_WRITE, s), buffer(data, data + len),
          closeAfterWrite(closeAfter)
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
        {
            if (worker.joinable())
                worker.join();
        }
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
    std::unordered_map<SOCKET, std::unique_ptr<Session>> sessions;
    std::mutex session_mutex;

    void log_io_operation(io_type type, SOCKET s, const std::string &status,
                          DWORD bytes = 0)
    {
        const char *type_str = "";
        switch (type)
        {
        case IO_ACCEPT:
            type_str = "ACCEPT";
            break;
        case IO_READ:
            type_str = "READ";
            break;
        case IO_WRITE:
            type_str = "WRITE";
            break;
        default:
            type_str = "UNKNOWN";
            break;
        }
        LOG("[SOCKET:" << s << "][" << type_str << "] " << status
                       << (bytes ? " Bytes:" + std::to_string(bytes) : ""));
    }

    Session *get_session(SOCKET s)
    {
        std::lock_guard<std::mutex> lock(session_mutex);
        auto it = sessions.find(s);
        return (it != sessions.end()) ? it->second.get() : nullptr;
    }

    void remove_session(SOCKET s)
    {
        std::lock_guard<std::mutex> lock(session_mutex);
        if (sessions.erase(s))
        {
            LOG("[CLIENT:" << s << "] Session destroyed");
        }
    }

    void post_accept()
    {
        auto op = new IocpAcceptOp(listen_socket_);
        LOG("[LISTEN:" << listen_socket_ << "] Posting AcceptEx");
        DWORD bytes_received = 0;
        BOOL result =
            pfnAcceptEx_(listen_socket_, op->client_socket, op->buffer, 0,
                         IocpAcceptOp::MIN_ADDR_LENGTH, IocpAcceptOp::MIN_ADDR_LENGTH,
                         &bytes_received, (OVERLAPPED *)op);

        if (!result && WSAGetLastError() != ERROR_IO_PENDING)
        {
            LOG("AcceptEx failed: " << WSAGetLastError());
            delete op;
        }
    }

    void post_read(SOCKET socket, Session *session)
    {
        auto op = new IocpReadOp(socket, session);
        DWORD flags = 0;
        LOG("[CLIENT:" << socket << "] Posting read operation");
        if (WSARecv(socket, &op->wsabuf, 1, nullptr, &flags, op, nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                LOG("[CLIENT:" << socket << "] Read failed: " << WSAGetLastError());
                closesocket(socket);
                delete op;
            }
        }
    }

    void post_write_chunk(Session *session)
    {
        constexpr size_t CHUNK_SIZE = 8;
        size_t remaining = session->write_buffer.size() - session->write_offset;

        if (remaining == 0)
        {
            LOG("[CLIENT:" << session->socket << "] Write complete");
            if (session->keep_alive)
            {
                LOG("[CLIENT:" << session->socket << "] Keep-alive, resetting session");
                session->write_offset = 0;
                session->read_buffer.clear();
                session->headers_parsed = false;
                post_read(session->socket, session);
            }
            else
            {
                LOG("[CLIENT:" << session->socket << "] Closing connection");
                closesocket(session->socket);
                remove_session(session->socket);
            }
            return;
        }

        size_t send_size = (remaining > CHUNK_SIZE) ? CHUNK_SIZE : remaining;
        bool final_chunk = (send_size == remaining);

        auto op = new IocpWriteOp(session->socket,
                                  &session->write_buffer[session->write_offset],
                                  send_size, !session->keep_alive && final_chunk);

        session->write_offset += send_size;
        LOG("[CLIENT:" << session->socket << "] Writing chunk ["
                       << (session->write_offset - send_size) << "-"
                       << session->write_offset << "/" << session->write_buffer.size()
                       << "]");

        DWORD sent = 0;
        if (WSASend(session->socket, &op->wsabuf, 1, &sent, 0, op, nullptr) ==
            SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
            {
                LOG("[CLIENT:" << session->socket
                               << "] Write failed: " << WSAGetLastError());
                closesocket(session->socket);
                delete op;
            }
        }
    }

    void parse_http_headers(Session *session, const std::string &request)
    {
        // 解析Connection头
        std::string connection_header = "Connection: keep-alive";
        session->keep_alive = (request.find(connection_header) != std::string::npos);

        LOG("[CLIENT:" << session->socket << "] Keep-Alive: "
                       << (session->keep_alive ? "Enabled" : "Disabled"));
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

        LOG("Server started on port 8080");
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
                LOG("Completion error: " << err);
                continue;
            }

            auto op = static_cast<IocpOperationBase *>(overlapped);
            log_io_operation(op->type, op->socket, "Completed", bytes);

            switch (op->type)
            {
            case IO_ACCEPT: {
                auto accept_op = static_cast<IocpAcceptOp *>(op);
                SOCKET client = accept_op->client_socket;
                accept_op->client_socket = INVALID_SOCKET;

                {
                    std::lock_guard<std::mutex> lock(session_mutex);
                    sessions[client] = std::make_unique<Session>(client);
                    LOG("[CLIENT:" << client << "] Session created");
                }

                setsockopt(client, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                           reinterpret_cast<char *>(&listen_socket_), sizeof(SOCKET));
                CreateIoCompletionPort(reinterpret_cast<HANDLE>(client), iocp_, 0, 0);
                post_accept();
                post_read(client, sessions[client].get());
                delete accept_op;
                break;
            }

            case IO_READ: {
                auto read_op = static_cast<IocpReadOp *>(op);
                Session *session = read_op->session;

                if (bytes == 0)
                {
                    LOG("[CLIENT:" << read_op->socket << "] Connection closed by client");
                    remove_session(read_op->socket);
                    closesocket(read_op->socket);
                    delete read_op;
                    break;
                }

                session->read_buffer.insert(session->read_buffer.end(), read_op->buffer,
                                            read_op->buffer + bytes);
                LOG("[CLIENT:" << read_op->socket << "] Buffered data: "
                               << session->read_buffer.size() << " bytes");

                if (!session->headers_parsed)
                {
                    const char *end_seq = "\r\n\r\n";
                    auto begin = session->read_buffer.begin();
                    auto end = session->read_buffer.end();
                    auto pos = std::search(begin, end, end_seq, end_seq + 4);

                    if (pos != end)
                    {
                        std::string headers(session->read_buffer.begin(), pos + 4);
                        parse_http_headers(session, headers);
                        session->headers_parsed = true;
                    }
                }

                if (session->headers_parsed)
                {
                    thread_pool.submit([this, read_op, session] {
                        std::string request_data(session->read_buffer.begin(),
                                                 session->read_buffer.end());

                        // 解析请求路径
                        std::string path;
                        size_t method_end = request_data.find(' ');
                        if (method_end != std::string::npos)
                        {
                            size_t path_end = request_data.find(' ', method_end + 1);
                            if (path_end != std::string::npos)
                            {
                                path = request_data.substr(method_end + 1,
                                                           path_end - method_end - 1);
                                size_t query_pos = path.find('?');
                                if (query_pos != std::string::npos)
                                {
                                    path = path.substr(0, query_pos);
                                }
                                if (!path.empty() && path[0] == '/')
                                {
                                    path = path.substr(1);
                                }
                            }
                        }

                        LOG("[CLIENT:" << read_op->socket << "] Request path: '" << path
                                       << "'");

                        std::string content = path.empty() ? "Hello World!" : path;
                        std::string response = "HTTP/1.1 200 OK\r\n"
                                               "Content-Type: text/plain\r\n"
                                               "Content-Length: " +
                                               std::to_string(content.size()) +
                                               "\r\n\r\n" + content;

                        if (session->keep_alive)
                        {
                            response.insert(response.find("\r\n\r\n"),
                                            "\r\nConnection: keep-alive");
                        }

                        session->write_buffer.assign(response.begin(), response.end());
                        session->write_offset = 0;
                        LOG("[CLIENT:" << read_op->socket << "] Response prepared ("
                                       << session->write_buffer.size() << " bytes)");
                        post_write_chunk(session);
                        delete read_op;
                    });
                }
                else
                {
                    LOG("[CLIENT:" << read_op->socket
                                   << "] Partial headers, continue reading");
                    post_read(read_op->socket, session);
                    delete read_op;
                }
                break;
            }

            case IO_WRITE: {
                auto write_op = static_cast<IocpWriteOp *>(op);
                thread_pool.submit([this, write_op] {
                    if (auto session = get_session(write_op->socket))
                    {
                        if (!write_op->closeAfterWrite)
                        {
                            post_write_chunk(session);
                        }
                        else
                        {
                            LOG("[CLIENT:" << write_op->socket << "] Final chunk sent");
                            if (session->keep_alive)
                            {
                                LOG("[CLIENT:" << write_op->socket
                                               << "] Keep-alive connection");
                                session->headers_parsed = false;
                                session->read_buffer.clear();
                                post_read(write_op->socket, session);
                            }
                            else
                            {
                                LOG("[CLIENT:" << write_op->socket
                                               << "] Closing connection");
                                closesocket(write_op->socket);
                                remove_session(write_op->socket);
                            }
                        }
                    }
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

/**
 Keep-alive 让浏览器自动关闭
[20:43:55][18708] [SOCKET:292][WRITE] Completed Bytes:4
[20:43:55][27512] [CLIENT:292] Write complete
[20:43:55][27512] [CLIENT:292] Keep-alive, resetting session
[20:43:55][27512] [CLIENT:292] Posting read operation
[20:45:30][18708] [SOCKET:268][READ] Completed
[20:45:30][18708] [CLIENT:268] Connection closed by client
[20:45:30][18708] [CLIENT:268] Session destroyed
 */
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