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

}; // namespace mcs::abnf::uri
