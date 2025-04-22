#pragma once

#include "./__obs_no_ws_ctl.hpp"

namespace mcs::abnf::imf
{
    // obs-ctext       =   obs-NO-WS-CTL
    constexpr abnf_result auto obs_ctext(octet_param_in c) noexcept
    {
        return obs_NO_WS_CTL(c);
    }
} // namespace mcs::abnf::imf