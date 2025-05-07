#pragma once

#include "./__received_protocol.hpp"
#include "./__received_by.hpp"
#include "./__comment.hpp"
#include "./__rws.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    /**
     * Via = [ ( received-protocol RWS received-by [ RWS comment ] ) *( OWS
             "," OWS ( received-protocol RWS received-by [ RWS comment ] ) ) ]
     *
     */
    using Via = sequence<optional<
        sequence<received_protocol, RWS, received_by, optional<sequence<RWS, comment>>,
                 zero_or_more<sequence<OWS, Char<','>, OWS, received_protocol, RWS,
                                       received_by, optional<sequence<RWS, comment>>>>>>>;
}; // namespace mcs::abnf::http