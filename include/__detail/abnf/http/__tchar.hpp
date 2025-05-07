#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // tchar = "!" / "#" / "$" / "%" / "&" / "'" / "*" / "+" / "-" / "." /
    // "^" / "_" / "`" / "|" / "~" / DIGIT / ALPHA
    using tchar = alternative<any_of<'!', '#', '$', '%', '&', '\'', '*', '+', '-', '.',
                                     '^', '_', '`', '|', '~'>,
                              DIGIT, ALPHA>;

}; // namespace mcs::abnf::http