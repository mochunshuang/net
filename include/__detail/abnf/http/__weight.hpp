#pragma once

#include "./__qvalue.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // OWS = *( SP / HTAB )
    // weight = OWS ";" OWS "q=" qvalue
    using weight = sequence<OWS, CharSensitive<';'>, OWS, CharInsensitive<'q'>,
                            CharSensitive<'='>, qvalue>;
}; // namespace mcs::abnf::http