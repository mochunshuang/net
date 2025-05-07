#pragma once

#include "./__alphanum.hpp"
#include "./__singleton.hpp"

namespace mcs::abnf::tfil
{
    // extension     = singleton 1*("-" (2*8alphanum))
    using extension = // NOLINTNEXTLINE
        sequence<singleton, one_or_more<sequence<Char<'-'>, repetition<2, 8, alphanum>>>>;
}; // namespace mcs::abnf::tfil