#pragma once

#include "./__segment.hpp"

namespace mcs::abnf::http
{
    // absolute-path = 1*( "/" segment )
    using absolute_path = one_or_more<sequence<Char<'/'>, segment>>;
}; // namespace mcs::abnf::http