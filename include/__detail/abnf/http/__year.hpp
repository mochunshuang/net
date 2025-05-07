#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // year = 4DIGIT
    using year = times<4, DIGIT>;
}; // namespace mcs::abnf::http