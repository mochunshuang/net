#pragma once

#include "./__chunk.hpp"
#include "./__last_chunk.hpp"
#include "./__trailer_section.hpp"

namespace mcs::protocol::http::rules
{
    // chunked-body = *chunk last-chunk trailer-section CRLF
    using chunk_body = abnf::sequence<abnf::zero_or_more<chunk>, last_chunk,
                                      trailer_section, abnf::CRLF>;
}; // namespace mcs::protocol::http::rules