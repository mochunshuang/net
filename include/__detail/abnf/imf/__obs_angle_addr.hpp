#pragma once

#include "./__addr_spec.hpp"
#include "./__cfws.hpp"
#include "./__obs_route.hpp"

namespace mcs::abnf::imf
{
    // obs-angle-addr  =   [CFWS] "<" obs-route addr-spec ">" [CFWS]
    using obs_angle_addr = sequence<optional<CFWS>, CharSensitive<'<'>, obs_route,
                                    addr_spec, CharSensitive<'>'>, optional<CFWS>>;
}; // namespace mcs::abnf::imf