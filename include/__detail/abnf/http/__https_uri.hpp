#pragma once

#include "./__authority.hpp"
#include "./__query.hpp"
#include "./__path_abempty.hpp"

namespace mcs::abnf::http
{
    // https-URI = "https://" authority path-abempty [ "?" query ]
    using https_URI = sequence<String<"https://">, authority, path_abempty,
                               optional<sequence<Char<'?'>, query>>>;
}; // namespace mcs::abnf::http