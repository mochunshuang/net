#pragma once

#include "./__field_name.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Trailer = [ field-name *( OWS "," OWS field-name ) ]
    using Trailer = optional<
        sequence<field_name, zero_or_more<sequence<OWS, Char<','>, OWS, field_name>>>>;
}; // namespace mcs::abnf::http