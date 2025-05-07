#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // first-pos = 1*DIGIT
    using first_pos = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http