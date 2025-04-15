#pragma once

#include "../__core_rules.hpp"
#include <array>

namespace mcs::abnf::uri
{
    // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
    constexpr bool sub_delims(octet_param_in c) noexcept
    {
        static_assert(std::numeric_limits<decltype(auto(c))>::max() == 255); // NOLINT
        static_assert(std::numeric_limits<decltype(auto(c))>::min() == 0);   // NOLINT
        constexpr auto k_sub_delims = []() consteval {
            std::array<bool, 256> arr{};
            const std::array<OCTET, 11> k_chars = {'!', '$', '&', '\'', '(', ')',
                                                   '*', '+', ',', ';',  '='};
            for (const auto &i : k_chars)
            {
                arr[i] = true; // NOLINT
            }
            return arr;
        }();
        static_assert(k_sub_delims['!']);
        return k_sub_delims[c]; // NOLINT
    }
}; // namespace mcs::abnf::uri