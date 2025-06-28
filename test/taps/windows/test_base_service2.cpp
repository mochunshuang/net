
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

    ex::spawn(
        ex::starts_on(
            pool.get_scheduler(),
            [=](auto &service) noexcept -> ex::lazy<bool> {
                int times = 1;
                while (times-- > 0)
                {
                    char buffer[2 * mcs::net::io::windows::ADDRESS_BUFFER_SIZE];
                    mcs::net::io::windows::io_operation_accept_context p{
                        mcs::net::io::windows::io_operation_context_base{
                            service.make_raw_socket(ep),
                            {.len = 2 * mcs::net::io::windows::ADDRESS_BUFFER_SIZE,
                             .buf = buffer}},
                        listen_socket};
                    auto connect = co_await service.make_connection(p);

                    assert(connect.socket == p.socket);
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
                }
                service.close_service(listen_socket);
                service.shutdown();
                co_return true;
            }(base_service) | ex::then([&](auto ret) noexcept {
                                               std::println("task done: {}", ret);
                                           })),
        scope.get_token());

    base_service.run();
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