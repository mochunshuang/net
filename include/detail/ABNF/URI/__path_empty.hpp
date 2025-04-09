#pragma once

#include "../__core_rules.hpp"

namespace mcs::ABNF::URI
{
    /**
   A rule of the form:

         <n>element

   is equivalent to

         <n>*<n>element

   That is, exactly <n> occurrences of <element>.  Thus, 2DIGIT is a
   2-digit number, and 3ALPHA is a string of three alphabetic
   characters.
     */

    // path-empty    = 0<pchar> ; path-empty      ; zero characters
    constexpr bool path_empty(default_span_t sp) noexcept
    {
        return sp.empty();
    }

}; // namespace mcs::ABNF::URI