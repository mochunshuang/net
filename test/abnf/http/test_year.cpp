#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf::http;

int main()
{
    TEST("base") = [] {
        constexpr auto y = "9999"_span;
        static_assert(year(y));

        {
            constexpr auto y = "999"_span;
            static_assert(not year(y));
        }
        {
            constexpr auto y = "999a"_span;
            static_assert(not year(y));
        }
    };
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND