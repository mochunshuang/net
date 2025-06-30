
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

    ex::spawn(ex::starts_on(
                  pool.get_scheduler(),
                  [=](auto &service) noexcept -> ex::lazy<bool> {
                      auto c = co_await service.make_http_connection();

                      co_return true;
                  }(http) | ex::then([&](auto ret) noexcept {
                                                     std::println("task done: {}", ret);

                                                     base_service.shutdown(
                                                         thead_count); // NOTE: ASYNC
                                                 })),
              scope.get_token());

    // base_service.run();
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