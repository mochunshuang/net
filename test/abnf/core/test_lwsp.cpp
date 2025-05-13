
#include "../test_abnf.hpp"
#include <array>
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("lwsp") = [] {
        constexpr auto lwsp = [](auto ctx) constexpr {
            return mcs::abnf::LWSP{}(ctx);
        };
        static_assert(lwsp("0"_ctx));
        static_assert(lwsp("0"_ctx).value() == 0);

        static_assert(lwsp("9"_ctx));
        static_assert(lwsp("A"_ctx));
        static_assert(lwsp("F"_ctx));

        static_assert(lwsp("a"_ctx));

        static_assert(lwsp("G"_ctx));

        static constexpr std::array<octet, 1> test_char = {0xFF};
        constexpr std::span<const octet> s(test_char);
        static_assert(lwsp(detail::make_parser_ctx(s)));              // NOLINT
        static_assert(lwsp(detail::make_parser_ctx(s)).value() == 0); // NOLINT
        {
            static constexpr std::array<octet, 1> test_char = {sp_value}; // sp
            constexpr std::span<const octet> s(test_char);
            static_assert(lwsp(detail::make_parser_ctx(s)));              // NOLINT
            static_assert(lwsp(detail::make_parser_ctx(s)).value() == 1); // NOLINT
        }
        {
            static constexpr std::array<octet, 1> test_char = {htab_value}; // htab
            constexpr std::span<const octet> s(test_char);
            static_assert(lwsp(detail::make_parser_ctx(s)));              // NOLINT
            static_assert(lwsp(detail::make_parser_ctx(s)).value() == 1); // NOLINT
        }
        {
            static constexpr std::array<octet, 3> test_char = {cr_value, lf_value,
                                                               sp_value}; // cllf wsp
            constexpr std::span<const octet> s(test_char);
            static_assert(lwsp(detail::make_parser_ctx(s)));              // NOLINT
            static_assert(lwsp(detail::make_parser_ctx(s)).value() == 3); // NOLINT
        }
        {
            static constexpr std::array<octet, 3> test_char = {cr_value, lf_value,
                                                               htab_value}; // cllf wsp
            constexpr std::span<const octet> s(test_char);
            static_assert(lwsp(detail::make_parser_ctx(s)));              // NOLINT
            static_assert(lwsp(detail::make_parser_ctx(s)).value() == 3); // NOLINT
        }
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND