#pragma once

#include "./__obs_fws.hpp"

namespace mcs::abnf::imf
{
    // FWS             =   ([*WSP CRLF] 1*WSP) /  obs-FWS ;Folding white space
    using FWS = alternative<
        sequence<optional<sequence<zero_or_more<WSP>, CRLF>>, one_or_more<WSP>>, obs_FWS>;
}; // namespace mcs::abnf::imf