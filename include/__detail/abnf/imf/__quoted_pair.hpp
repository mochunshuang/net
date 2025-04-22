#pragma once

#include "../__core_rules.hpp"
#include "./__obs_qp.hpp"

namespace mcs::abnf::imf
{
    // quoted-pair     =   ("\" (VCHAR / WSP)) / obs-qp
    constexpr abnf_result auto quoted_pair(octet_param_in a, octet_param_in b) noexcept
    {
        return ((a == '\\' && (VCHAR(b) || WSP(b))) || obs_qp(a, b));
    }
} // namespace mcs::abnf::imf