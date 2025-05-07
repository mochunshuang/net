#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // Max-Forwards = 1*DIGIT
    using Max_Forwards = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http
