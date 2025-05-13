#pragma once

#include "./__field_line.hpp"

namespace mcs::protocol::http::rules
{
    // trailer-section = *( field-line CRLF )
    using trailer_section = abnf::zero_or_more<abnf::sequence<field_line, abnf::CRLF>>;
}; // namespace mcs::protocol::http::rules