#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // last-pos = 1*DIGIT
    using last_pos = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http
