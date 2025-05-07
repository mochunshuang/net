#pragma once

#include "../uri/__path_abempty.hpp"

namespace mcs::abnf::http
{
    // path-abempty = <path-abempty, see [URI], Section 3.3>
    using path_abempty = uri::path_abempty;
}; // namespace mcs::abnf::http
