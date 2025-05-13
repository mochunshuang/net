#pragma once

#include "./__uri_host.hpp"
#include "./__port.hpp"

namespace mcs::protocol::http::rules
{
    // authority-form = uri-host ":" port
    using authority_form = abnf::sequence<uri_host, abnf::Char<':'>, port>;
}; // namespace mcs::protocol::http::rules