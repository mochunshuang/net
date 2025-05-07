#pragma once

#include "./__dot_atom.hpp"
#include "./__domain_literal.hpp"
#include "./__obs_domain.hpp"

namespace mcs::abnf::imf
{
    // domain          =   dot-atom / domain-literal / obs-domain
    using domain = alternative<dot_atom, domain_literal, obs_domain>;
}; // namespace mcs::abnf::imf