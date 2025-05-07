#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // day = 2DIGIT
    using day = times<2, DIGIT>;
}; // namespace mcs::abnf::http