
#include "../test_abnf.hpp"

#include <iostream>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::operators;

int main()
{
    {
        consumed_result foo(const_parser_ctx) noexcept;
        struct Bar
        {
            consumed_result operator()(const_parser_ctx) noexcept;
        };
        auto lambda = [](const_parser_ctx) noexcept -> consumed_result {
            return {};
        };

        static_assert(not operable<decltype(foo)>); // 函数
        static_assert(operable<Bar>);               // 仿函数
        static_assert(operable<decltype(lambda)>);  // Lambda
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND