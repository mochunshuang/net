#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    /**
     * region        = 2ALPHA              ; ISO 3166-1 code
                     / 3DIGIT              ; UN M.49 code
     *
     */
    using region = alternative<times<2, ALPHA>, times<3, DIGIT>>;
}; // namespace mcs::abnf::tfil