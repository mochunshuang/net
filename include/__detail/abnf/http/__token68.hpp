#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // token68 = 1*( ALPHA / DIGIT / "-" / "." / "_" / "~" / "+" / "/" ) *"="
    using token68 = sequence<
        one_or_more<alternative<ALPHA, DIGIT, any_of<'-', '.', '_', '~', '+', '/'>>>,
        zero_or_more<Char<'='>>>;

}; // namespace mcs::abnf::http