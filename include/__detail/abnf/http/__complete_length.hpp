#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // complete-length = 1*DIGIT
    using complete_length = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http