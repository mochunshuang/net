#pragma once

#include "./__dot_atom_text.hpp"
#include "./__cfws.hpp"

namespace mcs::abnf::imf
{
    // dot-atom        =   [CFWS] dot-atom-text [CFWS]
    using dot_atom = sequence<optional<CFWS>, dot_atom_text, optional<CFWS>>;
}; // namespace mcs::abnf::imf