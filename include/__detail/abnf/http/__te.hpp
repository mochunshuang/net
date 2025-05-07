#pragma once

#include "./__t_codings.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // TE = [ t-codings *( OWS "," OWS t-codings ) ]
    using TE = optional<
        sequence<t_codings, zero_or_more<sequence<OWS, Char<','>, OWS, t_codings>>>>;
}; // namespace mcs::abnf::http