#pragma once

#include "../../../abnf/http/__obs_text.hpp"

namespace mcs::protocol::http::rules
{
    // obs-text = <obs-text, see [HTTP], Section 5.6.4>
    using obs_text = abnf::http::obs_text;
}; // namespace mcs::protocol::http::rules