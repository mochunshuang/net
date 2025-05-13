
#include "../test_abnf.hpp"
#include <array>
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("wsp") = [] {
        constexpr auto wsp = [](auto ctx) constexpr {
            return mcs::abnf::WSP{}(ctx);
        };
        static_assert(not wsp("0"_ctx));
        static_assert(not wsp("9"_ctx));
        static_assert(not wsp("A"_ctx));
        static_assert(not wsp("F"_ctx));

        static_assert(not wsp("a"_ctx));

        static_assert(not wsp("G"_ctx));

        static constexpr std::array<octet, 1> test_char = {0xFF};
        constexpr std::span<const octet> s(test_char);
        static_assert(not wsp(detail::make_parser_ctx(s))); // NOLINT
        {
            static constexpr std::array<octet, 1> test_char = {0x20}; // sp
            constexpr std::span<const octet> s(test_char);
            static_assert(wsp(detail::make_parser_ctx(s))); // NOLINT
        }
        {
            static constexpr std::array<octet, 1> test_char = {0x09}; // htb
            constexpr std::span<const octet> s(test_char);
            static_assert(wsp(detail::make_parser_ctx(s))); // NOLINT
        }
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND