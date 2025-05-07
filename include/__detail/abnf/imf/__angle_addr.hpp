#pragma once

#include "./__addr_spec.hpp"
#include "./__cfws.hpp"
#include "./__obs_angle_addr.hpp"

namespace mcs::abnf::imf
{
    // angle-addr = [CFWS] "<" addr-spec ">" [CFWS] / obs-angle-addr
    using angle_addr = alternative<
        sequence<optional<CFWS>, Char<'<'>, addr_spec, Char<'>'>, optional<CFWS>>,
        obs_angle_addr>;
}; // namespace mcs::abnf::imf