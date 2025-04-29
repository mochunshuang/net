
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("CRLF") = [] {
        constexpr auto crlf = [](auto ctx) constexpr {
            return mcs::abnf::CRLF{}(ctx);
        };
        constexpr auto cr = [](auto ctx) constexpr {
            return mcs::abnf::CR{}(ctx);
        };
        constexpr auto lf = [](auto ctx) constexpr {
            return static_cast<bool>(mcs::abnf::LF{}(ctx));
        };
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