#pragma once

#include "./__obs_domain_list.hpp"

namespace mcs::abnf::imf
{
    //  obs-route       =   obs-domain-list ":"
    using obs_route = sequence<obs_domain_list, CharSensitive<':'>>;
}; // namespace mcs::abnf::imf