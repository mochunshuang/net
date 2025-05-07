#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // hour = 2DIGIT
    using hour = times<2, DIGIT>;
}; // namespace mcs::abnf::http