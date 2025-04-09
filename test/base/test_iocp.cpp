
#include <chrono>
#include <iostream>
#include <thread>

#include "../draft/common/iocp.hpp"
// #include "../../third_party/include/execution.hpp"

struct ReturnType
{
    struct promise_type
    {
        // Note: promise_type => coroutine
        ReturnType get_return_object()
        {
            return ReturnType{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }
        std::suspend_always final_suspend() noexcept
        {
            std::cout << "final_suspend() 线程 ID: " << std::this_thread::get_id()
                      << '\n';
            return {};
        }
        std::suspend_always yield_value(int i) noexcept
        {
            this->value = i;
            return {};
        }
        void return_void()
        {
            std::cout << "return_void: now_value: " << value
                      << ",线程 ID: " << std::this_thread::get_id() << '\n';
        }
        void unhandled_exception() noexcept
        {
            exception = std::current_exception();
        }

        int value{};
        std::exception_ptr exception{};
    };
    using handle_type = std::coroutine_handle<promise_type>;
    ReturnType(handle_type h) : handle(h)
    {
        std::cout << "ReturnType(handle_type h) 线程 ID: " << std::this_thread::get_id()
                  << '\n';
    }
    ~ReturnType() noexcept
    {
        std::cout << "~ReturnType() 线程 ID: " << std::this_thread::get_id() << '\n';
        if (handle)
        {
            handle.destroy();
            handle = nullptr;
        }
    }

    // Note: coroutine record coroutine_handle
    std::coroutine_handle<promise_type> handle{};
};

int main()
{
    endpoint end{};
    iocp_context server(end);
    server.bind();
    server.listen();

    auto test_sndr = [&]() -> ReturnType {
        std::cout << "async_accept start \n";
        co_await async_accept(server);
        std::cout << "async_accept resume \n";
    };

    // h.handle.resume();
    std::jthread j([&] noexcept {
        //
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        auto h = test_sndr();
        h.handle.resume();
    });
    server.run();

    std::cout << "main done\n";
    return 0;
}