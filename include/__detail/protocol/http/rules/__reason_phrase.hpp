#pragma once

#include "./__obs_text.hpp"

namespace mcs::protocol::http::rules
{
    // reason-phrase = 1*( HTAB / SP / VCHAR / obs-text )
    using quoted_string =
        abnf::one_or_more<abnf::alternative<abnf::HTAB, abnf::SP, abnf::VCHAR, obs_text>>;
}; // namespace mcs::protocol::http::rules