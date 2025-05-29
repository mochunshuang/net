#include <any>
#include <csignal>
#include <functional>
#include <iostream>
#include <print>

#if defined(_MSC_VER)

#include "../test_head.hpp"

namespace ex = ::mcs::execution;
namespace net = ::mcs::net;

void base_test() noexcept
{
    bool then_called = false;
    auto scope_sender =
        ex::just(1, std::string("abc")) | ex::then([](auto, auto) noexcept {

        }) |
        ex::let_async_scope([](auto scope_token) noexcept {
            // NOLINTEND
            static_assert(ex::scope::async_scope_token<decltype(scope_token)>);
            return ex::just(2, 1.0);
        }) |
        ex::then([&](auto a, auto b) {
            EXPECT(a == 2);
            EXPECT(b == 1.0);
            then_called = true;
        });
    mcs::this_thread::sync_wait(scope_sender);
    EXPECT(then_called);
}

void base_test2() noexcept
{
    bool then_called = false;
    auto scope_sender =
        ex::just(1, std::string("abc")) | ex::then([](auto, auto) noexcept {

        }) |
        ex::let_async_scope([](auto scope_token) noexcept {
            // NOLINTEND
            static_assert(ex::scope::async_scope_token<decltype(scope_token)>);
            return ex::just(2);
        }) |
        ex::then([&](auto a) {
            EXPECT(a == 2);
            then_called = true;
        });
    mcs::this_thread::sync_wait(scope_sender);
    EXPECT(then_called);
}

void base_test3()
{
    auto snd = ex::just() | ex::then([]() {}) |
               ex::let_async_scope([](auto scope_token) noexcept {
                   // NOLINTEND
                   static_assert(ex::scope::async_scope_token<decltype(scope_token)>);
                   return ex::just(2, 1.0);
               }) |
               ex::then([](auto a, auto b) {
                   EXPECT(a == 2);
                   EXPECT(b == 1.0);
                   std::println("let_async_scope done");
               });
    mcs::this_thread::sync_wait(snd);
}

void base_test4()
{
    ex::static_thread_pool<1> pool;
    auto sch = pool.get_scheduler();
    bool then_called = false;
    auto snd =
        ex::just() | ex::then([]() {}) |
        ex::let_async_scope([&](auto scope_token) noexcept {
            // NOLINTEND
            static_assert(ex::scope::async_scope_token<decltype(scope_token)>);

            ex::spawn(ex::starts_on(sch, ex::just() | ex::then([&] {
                                             then_called = true;
                                             std::println("spawn  work on pool done");
                                         })),
                      scope_token);
            return ex::just();
        }) |
        ex::then([&]() {
            EXPECT(then_called);
            std::println("let_async_scope done");
        });
    mcs::this_thread::sync_wait(std::move(snd));
}

void base_test5()
{
    ex::static_thread_pool<1> pool;
    auto sch = pool.get_scheduler();
    bool then_called = false;
    auto snd = ex::just() | ex::then([]() {}) |
               ex::let_async_scope([&](auto scope_token) noexcept {
                   // NOLINTEND
                   static_assert(ex::scope::async_scope_token<decltype(scope_token)>);

                   ex::spawn(ex::starts_on(sch,
                                           [&]() noexcept -> ex::lazy<> {
                                               then_called = true;
                                               co_return;
                                           }()),
                             scope_token);
                   return ex::just();
               }) |
               ex::then([&]() {
                   EXPECT(then_called);
                   std::println("let_async_scope done");
               });
    mcs::this_thread::sync_wait(std::move(snd));
}

void base_test6()
{
    bool then_called = false;
    auto ret = [&]() noexcept -> ex::lazy<bool> {
        co_return true;
    }() | ex::then([&](auto ret) noexcept { then_called = true; });
    static_assert(ex::sender<decltype(ret)>);

    ex::counting_scope scope;
    ex::static_thread_pool<1> pool;

    ex::spawn(ex::starts_on(pool.get_scheduler(), std::move(ret)), scope.get_token());

    mcs::this_thread::sync_wait(scope.join());

    EXPECT(then_called);
}

inline static net::io::iocp_context *g_ctx = nullptr;
inline static void signal_handler(int signal) noexcept // NOLINT
{
    if (signal == SIGINT && g_ctx != nullptr) // 捕获Ctrl+C信号
    {
        std::println("\nReceived SIGINT (Ctrl+C). Exiting...\n");
        g_ctx->finish();
    }
}
inline static void set_signal_handler(net::io::iocp_context &ctx) noexcept // NOLINT
{
    (void)std::signal(SIGINT, signal_handler);
    g_ctx = &ctx;
}

int main()
{
    base_test();
    base_test2();
    base_test3();
    base_test4();
    base_test5();
    base_test6();

    net::io::iocp_context context;
    ex::counting_scope scope;
    ex::static_thread_pool<1> pool;
    set_signal_handler(context);

    auto ret = [&]() noexcept -> ex::lazy<bool> {
        while (not context.is_stopped())
        {
            auto conn = co_await context.make_accept();
            // auto conn = co_await ex::just(0);
        }
        co_return true;
    }() | ex::then([&](auto ret) noexcept {});

    ex::spawn(ex::starts_on(pool.get_scheduler(), std::move(ret)), scope.get_token());

    context.run();
    mcs::this_thread::sync_wait(scope.join());

    std::cout << "main done\n";
    return 0;
}

#else
int main()
{
    std::cout << "main done\n";
    return 0;
}
#endif