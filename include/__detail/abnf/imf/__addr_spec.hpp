#pragma once

#include "./__domain.hpp"
#include "./__local_part.hpp"

namespace mcs::abnf::imf
{
    // addr-spec       =   local-part "@" domain
    using addr_spec = sequence<local_part, Char<'@'>, domain>;
}; // namespace mcs::abnf::imf