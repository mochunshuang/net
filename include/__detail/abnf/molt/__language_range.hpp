#pragma once

#include "./__alphanum.hpp"

namespace mcs::abnf::molt
{
    // language-range   = (1*8ALPHA *("-" 1*8alphanum)) / "*"
    using language_range = alternative<
        Char<'*'>,
        sequence<repetition<1, 8, ALPHA>, // NOLINT // NOLINTNEXTLINE
                 zero_or_more<sequence<Char<'-'>, repetition<1, 8, alphanum>>>>>;
}; // namespace mcs::abnf::molt