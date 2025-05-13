#pragma once

#include "../../../abnf/http/__query.hpp"

namespace mcs::protocol::http::rules
{
    // query = <query, see [URI], Section 3.4>
    using query = ::mcs::abnf::http::query;
}; // namespace mcs::protocol::http::rules