#pragma once

#include "./__alphanum.hpp"

namespace mcs::abnf::tfil
{
    /**
     * variant       = 5*8alphanum         ; registered variants
                     / (DIGIT 3alphanum)
     *
     */
    using variant = // NOLINTNEXTLINE
        alternative<repetition<5, 8, alphanum>, sequence<DIGIT, times<3, alphanum>>>;
}; // namespace mcs::abnf::tfil