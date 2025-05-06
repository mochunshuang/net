#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    //   h16           = 1*4HEXDIG
    using h16 = repetition<1, 4, HEXDIG>;
}; // namespace mcs::abnf::uri