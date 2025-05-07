#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // obs-text = %x80-FF
    using obs_text = Range<0x80, 0xFF>; // NOLINT
}; // namespace mcs::abnf::http