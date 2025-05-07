#pragma once

#include "./__atext.hpp"

namespace mcs::abnf::imf
{
    // dot-atom-text   =   1*atext *("." 1*atext)
    using dot_atom_text =
        sequence<one_or_more<atext>,
                 zero_or_more<sequence<CharSensitive<'.'>, one_or_more<atext>>>>;
}; // namespace mcs::abnf::imf