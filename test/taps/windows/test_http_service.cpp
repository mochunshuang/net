
// NOLINTBEGIN
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

    ex::spawn(ex::starts_on(pool.get_scheduler(),
                            [=](auto &service) noexcept -> ex::lazy<bool> {
                                auto c = co_await service.make_http_connection();
                                co_return true;
                            }(http) | ex::then([&](auto ret) noexcept {
                                                               std::println(
                                                                   "task done: {}", ret);
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