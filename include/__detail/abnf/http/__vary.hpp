#pragma once

#include "./__field_name.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Vary = [ ( "*" / field-name ) *( OWS "," OWS ( "*" / field-name ) ) ]
    using Vary = optional<sequence<
        alternative<Char<'*'>, field_name>,
        zero_or_more<sequence<OWS, Char<','>, OWS, alternative<Char<'*'>, field_name>>>>>;
}; // namespace mcs::abnf::http