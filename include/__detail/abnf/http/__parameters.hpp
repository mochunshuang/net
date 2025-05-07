#pragma once

#include "./__parameter.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // OWS = *( SP / HTAB )
    // parameters = *( OWS ";" OWS [ parameter ] )
    using parameters = one_or_more<sequence<OWS, Char<';'>, OWS, optional<parameter>>>;
}; // namespace mcs::abnf::http