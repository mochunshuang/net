#pragma once

#include "./__word.hpp"

namespace mcs::abnf::imf
{
    // obs-local-part  =   word *("." word)
    using obs_local_part =
        sequence<word, zero_or_more<sequence<CharSensitive<'.'>, word>>>;
}; // namespace mcs::abnf::imf