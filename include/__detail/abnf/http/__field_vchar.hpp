#pragma once

#include "../__abnf.hpp"
#include "./__obs_text.hpp"

namespace mcs::abnf::http
{
    // field-vchar = VCHAR / obs-text
    using field_vchar = alternative<VCHAR, obs_text>;
}; // namespace mcs::abnf::http