#pragma once

#include "./__http_date.hpp"
#include "./__entity_tag.hpp"

namespace mcs::abnf::http
{
    // If-Range = entity-tag / HTTP-date
    using If_Range = alternative<entity_tag, HTTP_date>;
}; // namespace mcs::abnf::http
