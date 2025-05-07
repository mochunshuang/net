#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // suffix-length = 1*DIGIT
    using suffix_length = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http