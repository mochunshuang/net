#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // minute = 2DIGIT
    using minute = times<2, DIGIT>;
}; // namespace mcs::abnf::http
