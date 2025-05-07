#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::imf
{
    /**
     * atext           =   ALPHA / DIGIT /    ; Printable US-ASCII
                       "!" / "#" /        ;  characters not including
                       "$" / "%" /        ;  specials.  Used for atoms.
                       "&" / "'" /
                       "*" / "+" /
                       "-" / "/" /
                       "=" / "?" /
                       "^" / "_" /
                       "`" / "{" /
                       "|" / "}" /
                       "~"
     *
     */
    using atext = alternative<ALPHA, DIGIT,
                              any_of<'!', '#', '$', '%', '&', '\'', '*', '+', '-', '/',
                                     '=', '?', '^', '_', '`', '{', '|', '}', '~'>>;
}; // namespace mcs::abnf::imf