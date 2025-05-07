#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    //  pct-encoded   = "%" HEXDIG HEXDIG
    using pct_encoded = sequence<Char<'%'>, HEXDIG, HEXDIG>;
}; // namespace mcs::abnf::uri