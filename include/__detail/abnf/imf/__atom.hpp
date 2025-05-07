#pragma once

#include "./__atext.hpp"
#include "./__cfws.hpp"

namespace mcs::abnf::imf
{
    // atom            =   [CFWS] 1*atext [CFWS]
    using atom = sequence<optional<CFWS>, one_or_more<atext>, optional<CFWS>>;
}; // namespace mcs::abnf::imf