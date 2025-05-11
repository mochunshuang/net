#pragma once

#include "./__parameter.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // parameters = *( OWS ";" OWS [ parameter ] )
    using parameters = zero_or_more<sequence<OWS, Char<';'>, OWS, optional<parameter>>>;
}; // namespace mcs::abnf::http