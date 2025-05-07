#pragma once

#include "./__absolute_uri.hpp"
#include "./__partial_uri.hpp"

namespace mcs::abnf::http
{
    // Referer = absolute-URI / partial-URI
    using Referer = alternative<absolute_URI, partial_URI>;
}; // namespace mcs::abnf::http