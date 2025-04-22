#pragma once

#include "../__core_rules.hpp"
#include "./__obs_no_ws_ctl.hpp"

namespace mcs::abnf::imf
{
    // obs-qp          =   "\" (%d0 / obs-NO-WS-CTL / LF / CR)
    constexpr abnf_result auto obs_qp(octet_param_in a, octet_param_in b) noexcept
    {
        // NOLINTNEXTLINE
        return a == '\\' && (b == 0 || obs_NO_WS_CTL(b) || b == LF || b == CR); // NOLINT
    }
} // namespace mcs::abnf::imf