#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // second = 2DIGIT
    using second = times<2, DIGIT>;
}; // namespace mcs::abnf::http
