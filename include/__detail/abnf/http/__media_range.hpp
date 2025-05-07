#pragma once

#include "./__type.hpp"
#include "./__subtype.hpp"
#include "./__parameters.hpp"

namespace mcs::abnf::http
{
    // subtype = token
    // media-range = ( "*/*" / ( type "/*" ) / ( type "/" subtype ) ) parameters
    using media_range = sequence<
        alternative<StringSensitive<"*/*">, sequence<type, StringSensitive<"/*">>,
                    sequence<type, Char<'/'>, subtype>>,
        parameters>;
}; // namespace mcs::abnf::http
