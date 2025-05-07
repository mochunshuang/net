#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::molt
{
    // alphanum      = (ALPHA / DIGIT)     ; letters and numbers
    using alphanum = alternative<ALPHA, DIGIT>;
}; // namespace mcs::abnf::molt
