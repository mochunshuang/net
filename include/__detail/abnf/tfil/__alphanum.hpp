#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    // alphanum      = (ALPHA / DIGIT)     ; letters and numbers
    using alphanum = alternative<ALPHA, DIGIT>;
}; // namespace mcs::abnf::tfil