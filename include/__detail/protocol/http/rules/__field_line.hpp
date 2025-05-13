#pragma once

#include "./__field_name.hpp"
#include "./__field_value.hpp"
#include "./__ows.hpp"

namespace mcs::protocol::http::rules
{
    // field-line = field-name ":" OWS field-value OWS
    using field_line = abnf::sequence<field_name, abnf::Char<':'>, OWS, field_value, OWS>;
}; // namespace mcs::protocol::http::rules