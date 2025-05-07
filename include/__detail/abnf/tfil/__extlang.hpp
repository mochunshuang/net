#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    /**
     *  extlang       = 3ALPHA              ; selected ISO 639 codes
     *                  *2("-" 3ALPHA)      ; permanently reserved
     *
     */
    using extlang =
        sequence<times<3, ALPHA>, repetition<0, 2, sequence<Char<'-'>, times<3, ALPHA>>>>;
}; // namespace mcs::abnf::tfil