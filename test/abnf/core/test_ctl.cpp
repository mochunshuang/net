
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("ctl") = [] {
        constexpr auto ctl = [](auto ctx) constexpr {
            return mcs::abnf::CTL{}(ctx);
        };
        static_assert(ctl("\0x00"_ctx));
        static_assert(ctl("\0x1F"_ctx));
        static_assert(ctl("\0x7F"_ctx));

        static_assert(not(0x20 < 0x1F));

        static_assert(ctl("\0x20"_ctx));
        static_assert(not ctl("'\0x20'"_ctx));
        {
            static_assert("\0x20"_ctx.remain() == 4);
            static_assert("'\0x20'"_ctx.remain() == 6);

            static constexpr octet test_ctl = '\0x20';
            constexpr std::span<const octet> s(&test_ctl, 1);
            static_assert(not ctl(detail::make_parser_ctx(s)));
        }
        static_assert(not ctl("A"_ctx));
        static_assert(not ctl("0x80"_ctx));
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND