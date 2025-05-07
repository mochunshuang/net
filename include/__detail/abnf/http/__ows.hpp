#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // OWS = *( SP / HTAB )
    using OWS = zero_or_more<alternative<SP, HTAB>>;
}; // namespace mcs::abnf::http