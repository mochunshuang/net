#pragma once

#include "./__authority.hpp"
#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_empty.hpp"
#include "./__path_noscheme.hpp"

namespace mcs::abnf::uri
{
    /**
     *    relative-part = "//" authority path-abempty
                 / path-absolute
                 / path-noscheme
                 / path-empty
     *
     */
    using relative_part =
        alternative<sequence<StringSensitive<"//">, authority, path_abempty>,
                    path_absolute, path_noscheme, path_empty>;

}; // namespace mcs::abnf::uri