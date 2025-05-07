#pragma once

#include "./__alphanum.hpp"

namespace mcs::abnf::tfil
{
    // privateuse    = "x" 1*("-" (1*8alphanum))
    using privateuse =
        sequence<CharInsensitive<'x'>,
                 one_or_more<sequence<Char<'-'>, repetition<1, 8, alphanum>>>>; // NOLINT
}; // namespace mcs::abnf::tfil