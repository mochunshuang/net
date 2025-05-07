#pragma once

#include "./__relative_part.hpp"
#include "./__query.hpp"

namespace mcs::abnf::http
{
    // partial-URI = relative-part [ "?" query ]
    using partial_URI = sequence<relative_part, optional<sequence<Char<'?'>, query>>>;
}; // namespace mcs::abnf::http