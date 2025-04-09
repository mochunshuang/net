#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <string>
#include <sstream>
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
    size_t header_length = 0; // 新增字段跟踪头部长度

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

class HttpRequest
{
  public:
    explicit HttpRequest(const std::vector<char> &data)
    {
        std::string request_str(data.begin(), data.end());

        // 检查请求是否包含完整头部
        size_t header_end = request_str.find("\r\n\r\n");
        if (header_end == std::string::npos)
        {
            throw std::runtime_error("Incomplete headers");
        }

        // 解析请求行
        size_t first_line_end = request_str.find("\r\n");
        if (first_line_end == std::string::npos)
        {
            throw std::runtime_error("Invalid request line");
        }

        std::string request_line = request_str.substr(0, first_line_end);
        std::istringstream iss(request_line);
        iss >> method_ >> path_ >> version_;

        // 处理路径
        size_t query_pos = path_.find('?');
        if (query_pos != std::string::npos)
        {
            path_ = path_.substr(0, query_pos);
        }

        // 统一路径格式（保留前导斜杠）
        if (path_.empty() || path_[0] != '/')
        {
            path_ = "/" + path_;
        }

        // 解析头部
        size_t pos = first_line_end + 2;
        while (pos < header_end)
        {
            size_t line_end = request_str.find("\r\n", pos);
            if (line_end == std::string::npos || line_end > header_end)
                break;

            std::string header_line = request_str.substr(pos, line_end - pos);
            size_t colon = header_line.find(':');
            if (colon != std::string::npos)
            {
                std::string name = header_line.substr(0, colon);
                std::string value = header_line.substr(colon + 1);

                // 去除首尾空白
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                headers_[name] = value;
            }
            pos = line_end + 2;
        }

        keep_alive_ =
            (headers_.count("Connection") && headers_["Connection"] == "keep-alive");
    }

    const std::string &method() const
    {
        return method_;
    }
    const std::string &path() const
    {
        return path_;
    }
    const std::string &version() const
    {
        return version_;
    }
    std::string header(const std::string &name) const
    {
        auto it = headers_.find(name);
        return it != headers_.end() ? it->second : "";
    }
    bool keep_alive() const
    {
        return keep_alive_;
    }

  private:
    std::string method_;
    std::string path_;
    std::string version_;
    std::unordered_map<std::string, std::string> headers_;
    bool keep_alive_;
};

class HttpResponse
{
  public:
    HttpResponse() : status_code_(200), status_message_("OK"), keep_alive_(false) {}

    void set_status(int code, const std::string &message)
    {
        status_code_ = code;
        status_message_ = message;
    }

    void set_header(const std::string &name, const std::string &value)
    {
        headers_[name] = value;
    }

    void set_content(const std::string &content, const std::string &content_type)
    {
        content_ = content;
        headers_["Content-Type"] = content_type;
    }

    void set_keep_alive(bool keep_alive)
    {
        keep_alive_ = keep_alive;
    }

    std::vector<char> to_bytes() const
    {
        std::stringstream ss;
        ss << "HTTP/1.1 " << status_code_ << " " << status_message_ << "\r\n";

        // 自动设置必要头部
        if (headers_.find("Content-Length") == headers_.end())
        {
            ss << "Content-Length: " << content_.size() << "\r\n";
        }

        // 添加自定义头部
        for (const auto &header : headers_)
        {
            if (header.first != "Content-Length")
            { // 避免重复
                ss << header.first << ": " << header.second << "\r\n";
            }
        }

        ss << "Connection: " << (keep_alive_ ? "keep-alive" : "close") << "\r\n";
        ss << "\r\n" << content_;

        std::string response_str = ss.str();
        return {response_str.begin(), response_str.end()};
    }

  private:
    int status_code_;
    std::string status_message_;
    std::unordered_map<std::string, std::string> headers_;
    std::string content_;
    bool keep_alive_;
};

class iocp_context
{
  private:
    HANDLE iocp_;
    SOCKET listen_socket_;
    LPFN_ACCEPTEX pfnAcceptEx_;
    LPFN_GETACCEPTEXSOCKADDRS pfnGetAcceptExSockaddrs_;
    ThreadPool thread_pool;
    std::unordered_map<SOCKET, std::unique_ptr<Session>> sessions;
    std::mutex session_mutex;

    using Handler = std::function<void(const HttpRequest &, HttpResponse &)>;
    std::unordered_map<std::string, Handler> routes_;
    std::mutex routes_mutex_;

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
                session->header_length = 0; // 重置头部长度
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

    void add_route(const std::string &path, Handler handler)
    {
        std::lock_guard<std::mutex> lock(routes_mutex_);
        routes_[path] = handler;
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

                // 新增：检查是否收到完整头部的逻辑
                if (!session->headers_parsed)
                {
                    const char *end_seq = "\r\n\r\n";
                    auto begin = session->read_buffer.begin();
                    auto end = session->read_buffer.end();
                    auto pos = std::search(begin, end, end_seq, end_seq + 4);

                    if (pos != end)
                    {
                        session->headers_parsed = true;
                        session->header_length = pos + 4 - begin;
                    }
                    else
                    {
                        // 头部未接收完整，继续读取
                        post_read(read_op->socket, session);
                        delete read_op;
                        break;
                    }
                }

                // 仅当收到完整头部时才提交处理
                if (session->headers_parsed)
                {
                    thread_pool.submit([this, read_op, session] {
                        try
                        {
                            HttpRequest request(session->read_buffer);
                            HttpResponse response;
                            response.set_keep_alive(request.keep_alive());

                            {
                                std::lock_guard<std::mutex> lock(routes_mutex_);
                                auto it = routes_.find(request.path());
                                if (it != routes_.end())
                                {
                                    it->second(request, response);
                                }
                                else
                                {
                                    response.set_status(404, "Not Found");
                                    response.set_content("Resource not found",
                                                         "text/plain");
                                }
                            }

                            auto response_data = response.to_bytes();
                            session->write_buffer = std::move(response_data);
                            session->write_offset = 0;
                            post_write_chunk(session);
                        }
                        catch (const std::exception &e)
                        {
                            LOG("Error processing request: " << e.what());
                            HttpResponse response;
                            response.set_status(500, "Internal Error");
                            response.set_content("Internal server error", "text/plain");
                            auto response_data = response.to_bytes();
                            session->write_buffer = std::move(response_data);
                            session->write_offset = 0;
                            post_write_chunk(session);
                        }
                        delete read_op;
                    });
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
                                session->header_length = 0;
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

int main()
{
    try
    {
        iocp_context ctx;

        // 注册路由
        ctx.add_route("/", [](const HttpRequest &req, HttpResponse &res) {
            res.set_content("Welcome to the home page!", "text/plain");
        });

        ctx.add_route("/hello", [](const HttpRequest &req, HttpResponse &res) {
            res.set_content("Hello from the server!", "text/plain");
        });

        ctx.add_route("/data", [](const HttpRequest &req, HttpResponse &res) {
            res.set_content("{ \"status\": \"OK\" }", "application/json");
        });

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