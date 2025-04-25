
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("CRLF") = [] {
        // 有效BIT测试
        static_assert(not crlf("1"_ctx));
        static_assert(crlf("\r\n"_ctx));
        static_assert(crlf("\r\n"_ctx).value() == 2);
        static_assert(crlf("\r\naaaaa"_ctx).value() == 2);

        static_assert(not crlf("\r"_ctx));
        static_assert(not crlf("\n"_ctx));
        static_assert(not crlf("\n\r"_ctx));

        static_assert(cr("\r"_ctx));
        static_assert(lf("\n"_ctx));
        static_assert(cr("\r"_ctx).value() == 1);
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND