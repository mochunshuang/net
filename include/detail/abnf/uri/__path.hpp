#pragma once

#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_noscheme.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"

namespace mcs::abnf::uri
{
    /**
     * @brief
     *    path   = path-abempty    ; begins with "/" or is empty
     *           / path-absolute   ; begins with "/" but not "//"
     *           / path-noscheme   ; begins with a non-colon segment
     *           / path-rootless   ; begins with a segment
     *           / path-empty      ; zero characters
     */
    constexpr bool path(span_param_in sp) noexcept
    {
        return path_abempty(sp) || path_absolute(sp) || path_noscheme(sp) ||
               path_rootless(sp) || path_empty(sp);
    }
}; // namespace mcs::abnf::uri
