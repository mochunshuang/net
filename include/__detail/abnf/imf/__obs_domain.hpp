#pragma once

#include "./__atom.hpp"

namespace mcs::abnf::imf
{
    // obs-domain      =   atom *("." atom)
    using obs_domain = sequence<atom, zero_or_more<sequence<Char<'.'>, atom>>>;
}; // namespace mcs::abnf::imf