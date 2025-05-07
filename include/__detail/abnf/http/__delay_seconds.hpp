#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // delay-seconds = 1*DIGIT
    using delay_seconds = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http