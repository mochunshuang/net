
// NOLINTBEGIN
#if defined(_MSC_VER)

#include "../../../include/__detail/taps/services/windows/__base_service.hpp"
#include "../../../include/__detail/protocol/__ip.hpp"

#include <cassert>
#include <chrono>
#include <iostream>
#include <ostream>
#include <print>
#include <thread>

#include <Windows.h>

void start_base_service_with_pool(auto &service, auto &thread_pool, auto &scope) noexcept
{
    namespace ex = mcs::execution;
    for (std::size_t i = 0; i < thread_pool.size(); ++i)
    {
        ex::spawn(ex::schedule(thread_pool[i].get_scheduler()) | ex::then([&]() noexcept {
                      std::cout << "service.run thread_id: " << std::this_thread::get_id()
                                << '\n';
                      service.run();
                  }),
                  scope.get_token());
    }
}

int main()
{
    namespace ip = mcs::protocol::ip; // NOLINT
    namespace ex = mcs::execution;

    ip::tcp::endpoint ep(ip::address_v4::any(), 8080);

    mcs::net::services::windows::base_service<ip::tcp>
        base_service{}; // NOTE: init network
    // service.print_info(); // 打印
    // listen and bind => start service
    auto listen_socket = base_service.start_service(ep).value();

    ex::counting_scope scope;
    ex::static_thread_pool<1> pool;
    constexpr auto thead_count = 8;
    ex::static_thread_pool<thead_count> io_pool;

    // 添加内部客户端模拟函数
    auto internal_client = [&] {
        SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = ::inet_addr("127.0.0.1");
        addr.sin_port = ::htons(8080);

        if (::connect(sock, (sockaddr *)&addr, sizeof(addr)))
        {
            std::println("Internal client connect failed: {}", ::WSAGetLastError());
            ::closesocket(sock);
            return;
        }

        const char *request = "GET /internal HTTP/1.1\r\n"
                              "Host: localhost\r\n"
                              "Connection: close\r\n"
                              "User-Agent: InternalClient/1.0\r\n"
                              "\r\n";

        ::send(sock, request, std::strlen(request), 0);
        ::closesocket(sock);
        std::println(">>> Internal request sent");
    };

    ex::spawn(
        ex::starts_on(
            pool.get_scheduler(),
            [=](auto &service) noexcept -> ex::task<bool> {
                int times = 3;
                while (times-- > 0)
                {
                    std::jthread([&] {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        internal_client();
                    }).detach();

                    char buffer[2 * mcs::net::io::windows::ADDRESS_BUFFER_SIZE];
                    mcs::net::io::windows::io_operation_accept_context p{
                        mcs::net::io::windows::io_operation_context_base{
                            service.make_raw_socket(ep),
                            {.len = 2 * mcs::net::io::windows::ADDRESS_BUFFER_SIZE,
                             .buf = buffer}},
                        listen_socket};
                    auto connect = co_await service.make_connection(p);

                    assert(connect.socket == p.socket);
                    std::println(">>> accept socket: {}", connect.socket);
                    std::println("New connection: {}", connect.info.to_string());
                    std::println("Remote IP: {}", connect.info.remote.ip_address);
                    std::println("Remote Port: {}", connect.info.remote.port);
                    std::println("Local Port: {}", connect.info.local.port);

                    {
                        char buffer[4096];
                        auto ctx = co_await service.make_read(
                            {connect.socket, {.len = 4096, .buf = buffer}});
                        std::println("bytes_transferred: {} \n read: {}",
                                     ctx.bytes_transferred,
                                     std::string(buffer, ctx.bytes_transferred));
                    }
                    {
                        // 构造响应
                        std::string responseBody = "Hello World!";
                        std::string response = "HTTP/1.1 200 OK\r\n"
                                               "Content-Type: text/plain\r\n"
                                               "Content-Length: " +
                                               std::to_string(responseBody.length()) +
                                               "\r\n"
                                               "Connection: " +
                                               (false ? "keep-alive" : "close") +
                                               "\r\n"
                                               "\r\n" // 头与正文的空行
                                               + responseBody;
                        auto ctx = co_await service.make_write(
                            {connect.socket,
                             {.len = static_cast<::ULONG>(response.size()),
                              .buf = response.data()}});
                        std::println("write bytes_transferred: {} , response.size(): {} ",
                                     ctx.bytes_transferred, response.size());
                    }

                    // NOTE: close
                    service.close_socket(connect.socket);
                }
                co_return true;
            }(base_service) | ex::then([&](auto ret) noexcept {
                                               std::println("task done: {}", ret);
                                               base_service.close_service(listen_socket);
                                               base_service.shutdown(thead_count);
                                           })),
        scope.get_token());

    // NOTE: 证明了内部资源申请和释放 是安全的
    start_base_service_with_pool(base_service, io_pool, scope);
    mcs::this_thread::sync_wait(scope.join());

    std::cout << "main done\n";

    return 0;
}
#else

#include <iostream>

int main()
{
    std::cout << "main done\n";
    return 0;
}

#endif
// NOLINTEND