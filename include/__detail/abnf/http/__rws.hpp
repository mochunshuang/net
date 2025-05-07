#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // RWS = 1*( SP / HTAB )
    using RWS = one_or_more<alternative<SP, HTAB>>;
}; // namespace mcs::abnf::http