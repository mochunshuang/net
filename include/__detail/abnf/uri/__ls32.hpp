#pragma once

#include "./__h16.hpp"
#include "./__ipv4address.hpp"

namespace mcs::abnf::uri
{
    // ls32          = ( h16 ":" h16 ) / IPv4address
    using ls32 = alternative<sequence<h16, CharSensitive<':'>, h16>, IPv4address>;
}; // namespace mcs::abnf::uri