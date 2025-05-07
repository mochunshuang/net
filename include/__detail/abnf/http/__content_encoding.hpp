#pragma once

#include "./__content_coding.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Content-Encoding = [ content-coding *( OWS "," OWS content-coding )]
    using content_Encoding =
        optional<sequence<content_coding,
                          zero_or_more<sequence<OWS, Char<','>, OWS, content_coding>>>>;
    ;
}; // namespace mcs::abnf::http