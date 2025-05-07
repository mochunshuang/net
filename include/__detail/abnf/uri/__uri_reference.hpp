#pragma once

#include "./__uri.hpp"
#include "./__relative_ref.hpp"

namespace mcs::abnf::uri
{
    // URI-reference = URI / relative-ref
    using URI_reference = alternative<URI, relative_ref>;
}; // namespace mcs::abnf::uri