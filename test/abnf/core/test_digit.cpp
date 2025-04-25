
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("digit") = [] {
        static_assert(digit("0"_ctx));
        static_assert(digit("9"_ctx));

        static_assert(not digit("/"_ctx));
        static_assert(not digit(":"_ctx));
        static_assert(not digit("A"_ctx));
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND