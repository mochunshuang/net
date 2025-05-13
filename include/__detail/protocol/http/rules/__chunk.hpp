#pragma once

#include "./__chunk_data.hpp"
#include "./__chunk_ext.hpp"
#include "./__chunk_size.hpp"

namespace mcs::protocol::http::rules
{
    // chunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF
    using chunk = abnf::sequence<chunk_size, abnf::optional<chunk_ext>, abnf::CRLF,
                                 chunk_data, abnf::CRLF>;
}; // namespace mcs::protocol::http::rules