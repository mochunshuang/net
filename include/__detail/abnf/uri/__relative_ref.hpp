#pragma once

#include "./__relative_part.hpp"
#include "./__query.hpp"
#include "./__fragment.hpp"

namespace mcs::abnf::uri
{
    // relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
    using relative_ref = sequence<relative_part, optional<sequence<Char<'?'>, query>>,
                                  optional<sequence<Char<'#'>, fragment>>>;
}; // namespace mcs::abnf::uri