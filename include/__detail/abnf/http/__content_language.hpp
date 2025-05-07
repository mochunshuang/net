#pragma once

#include "./__language_tag.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Content-Language = [ language-tag *( OWS "," OWS language-tag ) ]
    using content_Language =
        optional<sequence<language_tag,
                          zero_or_more<sequence<OWS, Char<','>, OWS, language_tag>>>>;
}; // namespace mcs::abnf::http