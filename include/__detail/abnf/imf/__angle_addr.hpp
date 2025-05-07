#pragma once

#include "./__addr_spec.hpp"
#include "./__cfws.hpp"
#include "./__obs_angle_addr.hpp"

namespace mcs::abnf::imf
{
    // angle-addr = [CFWS] "<" addr-spec ">" [CFWS] / obs-angle-addr
    using angle_addr = alternative<sequence<optional<CFWS>, CharSensitive<'<'>, addr_spec,
                                            CharSensitive<'>'>, optional<CFWS>>,
                                   obs_angle_addr>;
}; // namespace mcs::abnf::imf