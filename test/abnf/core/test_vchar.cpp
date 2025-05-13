
#include "../test_abnf.hpp"
#include <array>
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("vchar") = [] {
        constexpr auto vchar = [](auto ctx) constexpr {
            return mcs::abnf::VCHAR{}(ctx);
        };
        static_assert(vchar("!"_ctx));
        static_assert(vchar("!"_ctx).value() == 1);

        static_assert(vchar("9"_ctx));
        static_assert(vchar("A"_ctx));
        static_assert(vchar("F"_ctx));
        static_assert(vchar("a"_ctx));
        static_assert(vchar("G"_ctx));

        static_assert(vchar("~"_ctx));
        static_assert(vchar("~xxx"_ctx).value() == 1);
        {
            {
                static constexpr std::array<octet, 1> test_char = {'\t'};
                static_assert(not vchar(detail::make_parser_ctx(test_char)));
            }
            {
                static constexpr std::array<octet, 1> test_char = {'\n'};
                static_assert(not vchar(detail::make_parser_ctx(test_char)));
            }
            {
                static constexpr std::array<octet, 1> test_char = {'\r'};
                static_assert(not vchar(detail::make_parser_ctx(test_char)));
            }
            {
                static constexpr std::array<octet, 1> test_char = {' '};
                static_assert(not vchar(detail::make_parser_ctx(test_char)));
            }
            {
                static constexpr std::array<octet, 1> test_char = {0};
                static_assert(not vchar(detail::make_parser_ctx(test_char)));
            }
        }

        static constexpr std::array<octet, 1> test_char = {0xFF};
        constexpr std::span<const octet> s(test_char);
        static_assert(not vchar(detail::make_parser_ctx(s)));                // NOLINT
        static_assert(vchar(detail::make_parser_ctx(s)).value_or(99) == 99); // NOLINT
        {
            static constexpr std::array<octet, 1> test_char = {sp_value}; // sp
            constexpr std::span<const octet> s(test_char);
            static_assert(not vchar(detail::make_parser_ctx(s)));                // NOLINT
            static_assert(vchar(detail::make_parser_ctx(s)).value_or(99) == 99); // NOLINT
        }
        {
            static constexpr std::array<octet, 1> test_char = {htab_value}; // htab
            constexpr std::span<const octet> s(test_char);
            static_assert(not vchar(detail::make_parser_ctx(s))); // NOLINT
        }
        {
            static constexpr std::array<octet, 3> test_char = {cr_value, lf_value,
                                                               sp_value}; // cllf wsp
            constexpr std::span<const octet> s(test_char);
            static_assert(not vchar(detail::make_parser_ctx(s))); // NOLINT
        }
        {
            static constexpr std::array<octet, 3> test_char = {cr_value, lf_value,
                                                               htab_value}; // cllf wsp
            constexpr std::span<const octet> s(test_char);
            static_assert(not vchar(detail::make_parser_ctx(s))); // NOLINT
        }
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND