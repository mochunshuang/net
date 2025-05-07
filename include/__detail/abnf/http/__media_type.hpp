#pragma once

#include "./__type.hpp"
#include "./__subtype.hpp"
#include "./__parameters.hpp"

namespace mcs::abnf::http
{
    // subtype = token
    // media-type = type "/" subtype parameters
    using media_type = sequence<type, Char<'/'>, subtype, parameters>;
}; // namespace mcs::abnf::http
