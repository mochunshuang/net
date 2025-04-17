#pragma once

#include "../__core_rules.hpp"
#include "./__obs_text.hpp"

namespace mcs::abnf::http
{
    // quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
    constexpr abnf_result auto quoted_pair(octet_param_in a, octet_param_in b) noexcept
    {
        return a == '\\' && (b == HTAB || b == SP || VCHAR(b) || obs_text(b));
    }
}; // namespace mcs::abnf::http