#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    // unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
    using unreserved = alternative<ALPHA, DIGIT, any_of<'-', '.', '_', '~'>>;
}; // namespace mcs::abnf::uri