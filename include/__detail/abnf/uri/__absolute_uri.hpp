#pragma once

#include "./__query.hpp"
#include "./__hier_part.hpp"
#include "./__scheme.hpp"

namespace mcs::abnf::uri
{
    // absolute-URI  = scheme ":" hier-part [ "?" query ]
    using absolute_URI =
        sequence<scheme, Char<':'>, hier_part, optional<sequence<Char<'?'>, query>>>;
}; // namespace mcs::abnf::uri