#pragma once

#include <algorithm>

namespace mcs::abnf::generate
{
    template <size_t N>
    struct FixedString
    {
        static constexpr size_t size = N;                    // NOLINT
        char value[N]{};                                     // NOLINT
        constexpr FixedString(const char (&str)[N]) noexcept // NOLINT
        {
            std::copy_n(str, N, value);
        }
    };
}; // namespace mcs::abnf::generate