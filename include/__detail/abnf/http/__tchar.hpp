#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
    // "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
    constexpr abnf_result auto tchar(octet_param_in c) noexcept
    {
        static_assert('_' == 0x5f); // NOLINT
        constexpr auto k_tchar = []() consteval {
            std::array<bool, 256> arr{};
            const std::array<OCTET, 15> k_chars = {'!',  '#', '$', '%', '&',
                                                   '\'', '*', '+', '-', '.',
                                                   '^',  '_', '`', '|', '~'};
            for (const auto &i : k_chars)
            {
                arr[i] = true; // NOLINT
            }
            return arr;
        }();

        return k_tchar[c] || DIGIT(c) || ALPHA(c); // NOLINT
    }

}; // namespace mcs::abnf::http