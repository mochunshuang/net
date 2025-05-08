#pragma once

#include "./__qvalue.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // weight = OWS ";" OWS "q=" qvalue
    using weight = sequence<OWS, Char<';'>, OWS, CharInsensitive<'q'>, Char<'='>, qvalue>;
}; // namespace mcs::abnf::http