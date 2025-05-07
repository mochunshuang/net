#pragma once

#include "./__display_name.hpp"
#include "./__group_list.hpp"

namespace mcs::abnf::imf
{
    // group           =   display-name ":" [group-list] ";" [CFWS]
    using group = sequence<display_name, Char<':'>, optional<group_list>, Char<';'>,
                           optional<CFWS>>;
}; // namespace mcs::abnf::imf