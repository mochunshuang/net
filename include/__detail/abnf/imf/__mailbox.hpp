#pragma once

#include "./__addr_spec.hpp"
#include "./__name_addr.hpp"

namespace mcs::abnf::imf
{
    // mailbox         =   name-addr / addr-spec
    using mailbox = alternative<name_addr, addr_spec>;
} ;// namespace mcs::abnf::imf