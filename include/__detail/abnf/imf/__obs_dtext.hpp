#pragma once

#include "./__obs_no_ws_ctl.hpp"
#include "./__quoted_pair.hpp"
namespace mcs::abnf::imf
{
    // obs-dtext       =   obs-NO-WS-CTL / quoted-pair
    using obs_dtext = alternative<obs_NO_WS_CTL, quoted_pair>;

}; // namespace mcs::abnf::imf