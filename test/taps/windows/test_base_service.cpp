
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

    {
        struct Object
        {
            Object()
            {
                std::println("Object()");
            }
            ~Object()
            {
                std::println("~Object()");
            }
        };
        auto rc = mcs::this_thread::sync_wait([] -> ex::lazy<int> { // NOLINT
            int times = 1;
            while (times-- > 0)
            {
                Object o;
                std::println("co_await start");
                auto v = co_await (ex::just(0) | ex::then([](int v) noexcept {
                                       std::this_thread::sleep_for(
                                           std::chrono::milliseconds(5));
                                       return v;
                                   }));
                std::println("co_await get result");
                assert(v == 0);
            }
            co_return 17;
        }());
    }

    ip::tcp::endpoint ep(ip::address_v4::any(), 8080);

    mcs::net::services::windows::base_service<ip::tcp> service{}; // NOTE: init network
    // service.print_info(); // 打印
    // listen and bind => start service
    auto listen_socket = service.start_service(ep).value();

    ex::counting_scope scope;
    ex::static_thread_pool<1> pool;

    // NOTE: lambda 引用 捕获有bug. lambda 被移动，那么lambda内部的引用，也会被移动
    // NOTE: std::move()
    /*
//NOTE: 引用本质是指针，难怪 引用不可移动的 对象，还能move，难关 指针的值是 0
===== base_service info start =====
service ptr: 0x4abc11f7d0
iocp: 0xb8
Listen socket: 264
LPFN_ACCEPTEX: 0x7ffc1470f6a0
LPFN_GETACCEPTEXSOCKADDRS: 0x7ffc14721bd0
===== base_service info end =====
===== base_service info start =====
service ptr: 0x4abc11f7b8
iocp: 0x300000000
Listen socket: 0
LPFN_ACCEPTEX: 0x7ffc00001f90
LPFN_GETACCEPTEXSOCKADDRS: 0xb8
===== base_service info end =====

*/

    // NOTE: lambda 作为 coroutine 的启动器，不能引用捕获。引用是指针，内部coroutine不共享
    auto ret = [&]() noexcept -> ex::lazy<bool> {
        int times = 1;
        while (times-- > 0)
        {
            char buffer[2 * mcs::net::io::windows::ADDRESS_BUFFER_SIZE];
            mcs::net::io::windows::io_operation_accept_context p{
                {service.make_raw_socket(ep),
                 {.len = 2 * mcs::net::io::windows::ADDRESS_BUFFER_SIZE, .buf = buffer}},
                listen_socket};
            // 测试复制
            auto p2 = p;
            assert(p.socket == p2.socket);
            assert(p.wsabuf.buf == p2.wsabuf.buf);
            assert(p.wsabuf.len == p2.wsabuf.len);
            auto &ctx = p;

            // service.print_info(); // 打印。 将会给外部的不同。 当ret 被 move
            // 再启动sender

            // NOTE: 投递 accept 操作 给IOCP
            // auto conn = co_await service.make_accept(p);

            // auto conn = co_await ex::just(0);
        }
        service.shutdown();
        co_return true;
    }() | ex::then([&](auto ret) noexcept { std::println("task done: {}", ret); });

    // NOTE: 因此，lambda不能捕获任何。coroutine 上下文只能从参数列表传递
#if (0)
    ex::spawn(ex::starts_on(pool.get_scheduler(), std::move(ret)), scope.get_token());
#else
    ex::spawn(
        ex::starts_on( // NOTE: coroutine 生成的lambda 可以按值捕获，但不能按引用捕获
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
                    // 测试复制
                    auto p2 = p;
                    assert(p.socket == p2.socket);
                    assert(p.wsabuf.buf == p2.wsabuf.buf);
                    assert(p.wsabuf.len == p2.wsabuf.len);
                    auto &ctx = p;

            // service.print_info(); // 打印
#if 0
                    // NOTE: 投递 accept 操作 给IOCP
                    auto conn = co_await service.make_accept(p);

                    std::println("co_await get result");

                    auto conn_info = service.connection_info(conn);
                    std::println("New connection: {}", conn_info.to_string());
                    std::println("Remote IP: {}", conn_info.remote.ip_address);
                    std::println("Remote Port: {}", conn_info.remote.port);
                    std::println("Local Port: {}", conn_info.local.port);

                    // auto conn = co_await ex::just(0);
                    std::memset(buffer, 0, sizeof(buffer));
#elseif 0

                    // 改写如下
                    auto sndr = [](auto &service,
                                   mcs::net::io::windows::io_operation_context_base
                                       ctx) noexcept {
                        return service.make_accept(ctx) |
                               ex::then([&](auto new_ctx) noexcept {
                                   using info_type =
                                       decltype(service.connection_info(new_ctx));
                                   struct connect_ctx
                                   {
                                       ::SOCKET socket;
                                       info_type info;
                                   };
                                   return connect_ctx{new_ctx.socket,
                                                      service.connection_info(new_ctx)};
                               });
                    }(service, p);
                    auto connect = co_await sndr;
#else
                    auto connect = co_await service.make_rawconnection(p);
#endif
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
            }(service) | ex::then([&](auto ret) noexcept {
                                               std::println("task done: {}", ret);
                                           })),
        scope.get_token());
#endif

    service.run();
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