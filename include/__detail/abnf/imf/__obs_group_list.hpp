#pragma once

#include "./__cfws.hpp"

namespace mcs::abnf::imf
{
    // obs-group-list  =   1*([CFWS] ",") [CFWS]
    using obs_group_list =
        sequence<one_or_more<sequence<optional<CFWS>, Char<','>>>, optional<CFWS>>;
}; // namespace mcs::abnf::imf