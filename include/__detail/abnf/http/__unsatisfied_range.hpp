#pragma once

#include "./__complete_length.hpp"

namespace mcs::abnf::http
{
    // unsatisfied-range = "*/" complete-length
    using unsatisfied_range = sequence<Char<'*'>, Char<'/'>, complete_length>;
}; // namespace mcs::abnf::http