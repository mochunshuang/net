#pragma once

#include "./__ows.hpp"
#include "./__entity_tag.hpp"

namespace mcs::abnf::http
{
    // If-None-Match = "*" / [ entity-tag *( OWS "," OWS entity-tag ) ]
    using If_None_Match = alternative<
        Char<'*'>,
        optional<sequence<entity_tag,
                          zero_or_more<sequence<OWS, Char<','>, OWS, entity_tag>>>>>;
}; // namespace mcs::abnf::http
