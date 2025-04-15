#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::uri
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
    constexpr bool path_empty(span_param_in sp) noexcept
    {
        return sp.empty();
    }

}; // namespace mcs::abnf::uri