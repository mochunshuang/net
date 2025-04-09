#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::URI
{
    // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
    constexpr bool sub_delims(octet_t c) noexcept
    {
        // NOLINTNEXTLINE
        constexpr bool k_table[256] = {
            ['!'] = true, ['$'] = true, ['&'] = true, ['\''] = true, // NOLINT
            ['('] = true, [')'] = true, ['*'] = true, ['+'] = true,
            [','] = true, [';'] = true, ['='] = true};
        return k_table[c]; // NOLINT
    }
}; // namespace mcs::ABNF::URI