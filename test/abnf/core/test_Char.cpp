
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("Char") = [] {
        static_assert(Char("0"_ctx));
        static_assert(Char("1"_ctx));

        static_assert(Char("A"_ctx)); // 大写字母

        static_assert(Char(" "_ctx));
        static_assert(not Char(""_ctx));

        static_assert(Char(" 84646a7前uqj"_ctx));

        constexpr octet k_nul = '\0'; // 或 '\x00'
        static_assert(not Char("\0"_ctx));
        static_assert(not Char("\x00"_ctx));
        static_assert(k_nul == '\x00');

        // Character too large for enclosing Character literal
        // typeclang(Character_too_large)
        static_assert(!Char("€"_ctx)); // 非 ASCII
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND