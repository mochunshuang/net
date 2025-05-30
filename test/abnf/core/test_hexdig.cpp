
#include "../test_abnf.hpp"
#include <array>
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("hexdig") = [] {
        constexpr auto hexdig = [](auto ctx) constexpr {
            return mcs::abnf::HEXDIG{}(ctx);
        };
        static_assert(hexdig("0"_ctx));
        static_assert(hexdig("9"_ctx));
        static_assert(hexdig("A"_ctx));
        static_assert(hexdig("F"_ctx));

        static_assert(hexdig("a"_ctx));

        static_assert(not hexdig("G"_ctx));

        static constexpr std::array<octet, 1> test_char = {0xFF};
        constexpr std::span<const octet> s(test_char);
        static_assert(not hexdig(detail::make_parser_ctx(s))); // NOLINT
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND