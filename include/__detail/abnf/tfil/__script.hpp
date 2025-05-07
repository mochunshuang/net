#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    //  script        = 4ALPHA              ; ISO 15924 code
    using script = times<4, ALPHA>;
}; // namespace mcs::abnf::tfil