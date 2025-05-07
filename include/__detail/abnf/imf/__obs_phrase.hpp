#pragma once

#include "./__word.hpp"
#include "./__cfws.hpp"

namespace mcs::abnf::imf
{
    // obs-phrase      =   word *(word / "." / CFWS)
    using obs_phrase =
        sequence<word, zero_or_more<alternative<word, CharSensitive<'.'>, CFWS>>>;
}; // namespace mcs::abnf::imf