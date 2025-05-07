#pragma once

#include "./__obs_no_ws_ctl.hpp"

namespace mcs::abnf::imf
{
    // obs-qp          =   "\" (%d0 / obs-NO-WS-CTL / LF / CR)
    using obs_qp = sequence<Char<'\\'>, alternative<Char<0>, obs_NO_WS_CTL, LF, CR>>;
}; // namespace mcs::abnf::imf