#pragma once

#include "./__obs_text.hpp"
#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    /**
    qdtext = HTAB / SP / "!" / %x23-5B ; '#'-'['
           / %x5D-7E ; ']'-'~'
           / obs-text
    */
    constexpr abnf_result auto qdtext(octet_param_in c) noexcept
    {
        return c == HTAB || c == SP || c == '!' || (c >= 0x23 && c <= 0x5B) || // NOLINT
               (c >= 0x5D && c <= 0x7E) || obs_text(c);                        // NOLINT
    }
}; // namespace mcs::abnf::http