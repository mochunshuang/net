#pragma once

#include "./__port.hpp"
#include "./__uri_host.hpp"

namespace mcs::abnf::http
{
    // Host = uri-host [ ":" port ]
    using Host = sequence<uri_host, optional<sequence<Char<':'>, port>>>;
}; // namespace mcs::abnf::http
