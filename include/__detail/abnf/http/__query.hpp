#pragma once

#include "../uri/__query.hpp"

namespace mcs::abnf::http
{
    // query = <query, see [URI], Section 3.4>
    using query = uri::query;
}; // namespace mcs::abnf::http