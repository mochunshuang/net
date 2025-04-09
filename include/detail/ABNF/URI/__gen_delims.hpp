#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::URI
{
    // gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
    constexpr bool gen_delims(octet_t c) noexcept
    {
        static_assert(0x5d == ']'); // NOLINT // NOLINTNEXTLINE
        constexpr bool k_table[256] = {
            [':'] = true, ['/'] = true,  ['?'] = true, ['#'] = true, // NOLINT
            ['['] = true, [0x5d] = true, ['@'] = true};
        static_assert(k_table[0x5d]); // NOLINT
        return k_table[c];            // NOLINT
    }
}; // namespace mcs::ABNF::URI