#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // Content-Length = 1*DIGIT
    using Content_Length = one_or_more<DIGIT>;
}; // namespace mcs::abnf::http