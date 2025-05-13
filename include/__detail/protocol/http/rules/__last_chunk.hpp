#pragma once

#include "./__chunk_ext.hpp"

namespace mcs::protocol::http::rules
{
    // last-chunk = 1*"0" [ chunk-ext ] CRLF
    using last_chunk = abnf::sequence<abnf::one_or_more<abnf::Char<'0'>>,
                                      abnf::optional<chunk_ext>, abnf::CRLF>;
}; // namespace mcs::protocol::http::rules