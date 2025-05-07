#pragma once

#include "./__authority.hpp"
#include "./__query.hpp"
#include "./__path_abempty.hpp"

namespace mcs::abnf::http
{
    // http-URI = "http://" authority path-abempty [ "?" query ]
    using http_URI = sequence<String<"http://">, authority, path_abempty,
                              optional<sequence<Char<'?'>, query>>>;
}; // namespace mcs::abnf::http
