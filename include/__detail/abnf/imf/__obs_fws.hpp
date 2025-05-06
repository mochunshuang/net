#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::imf
{
    // obs-FWS         =   1*WSP *(CRLF 1*WSP)
    using obs_FWS =
        sequence<one_or_more<WSP>, zero_or_more<sequence<CRLF, one_or_more<WSP>>>>;
}; // namespace mcs::abnf::imf