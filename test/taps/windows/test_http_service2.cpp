
// NOLINTBEGIN
#include <string>
#if defined(_MSC_VER)

#include "../../../include/__detail/taps/services/windows/__base_service.hpp"
#include "../../../include/__detail/taps/services/http/__http_service.hpp"
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
    // mcs::net::services::http::http_service<decltype(base_service)>::connect_count_config
    //     cfg{.init_connect_count = 1024};
    mcs::net::services::http::http_service http{base_service,
                                                {.init_connect_count = 1024}};
    if (not http.start({.local_endpoint = ep}))
    {
        std::println("http.start: error");
    }

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
                std::println(">>> spawn start thead_id: {}", std::this_thread::get_id());
                // NOTE: 浏览器默认两次请求，拉起服务
                int count = 1;
                while (count--)
                {
                    std::jthread([&] {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        internal_client();
                    }).detach();

                    // NOTE: make_http_connection 允许连接。相应已经建立的连接
                    auto http_connect = co_await service.make_http_connection();

                    {
                        auto rawconnect = http_connect.dependent_connect();
                        std::println("New connection: {}", rawconnect.info.to_string());
                        std::println("Remote IP: {}", rawconnect.info.remote.ip_address);
                        std::println("Remote Port: {}", rawconnect.info.remote.port);
                        std::println("Local Port: {}", rawconnect.info.local.port);
                    }

                    // NOTE: 允许已经建立的连接发起请求
                    //  接收： 请求。打印http 报文
                    {
                        char buffer[4096];
                        auto bytes_transferred =
                            co_await http_connect.aync_read(buffer, 4090);
                        std::println("[read bytes_transferred]: {} \n{}",
                                     bytes_transferred,
                                     std::string(buffer, bytes_transferred));
                        std::println(">>> read after thead_id: {}",
                                     std::this_thread::get_id());
                    }

                    {
                        // 构造响应：让浏览器打印
                        // NOTE: 最后浏览器影响影响：”Hello World!: 0“
                        std::string responseBody =
                            "Hello World!: " + std::to_string(count);
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
                        auto response_szie = response.size();
                        std::vector<char> vec;
                        vec.reserve(response.size() + 1); // +1 为了包含字符串结束符 '\0'

                        // 使用 std::move 避免拷贝
                        vec.assign(std::make_move_iterator(response.begin()),
                                   std::make_move_iterator(response.end()));
                        auto bytes_transferred =
                            co_await http_connect.aync_write(std::move(vec));
                        std::println("write bytes_transferred: {} , response.size(): {} ",
                                     bytes_transferred, response_szie);
                        std::println(">>> write after thead_id: {}",
                                     std::this_thread::get_id());
                    }
                    service.close_http_connection(http_connect);
                }
                std::println(">>> spawn end thead_id: {}", std::this_thread::get_id());

                co_return true;
            }(http) | ex::then([&http, &base_service](auto ret) noexcept {
                                               std::println("task done: {}", ret);
                                               std::println("base_service: {}",
                                                            (ptrdiff_t)&base_service);

                                               // NOTE: 原因是stop()有返回值但是没有
                                               // return 。就变成未定义了
                                               //  NOTE: 封装了 close_service 也能崩溃？？
                                               http.stop(); // 开启就崩溃

                                               //  这样就不崩溃，很不合理，为什么？
                                               //    base_service.close_service(
                                               //        http.listenSocket());
                                               base_service.shutdown(1);
                                           })),
        scope.get_token());

    base_service.run();
    // start_base_service_with_pool(base_service, io_pool, scope);
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