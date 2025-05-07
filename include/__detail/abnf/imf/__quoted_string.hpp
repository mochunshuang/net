#pragma once

#include "./__qcontent.hpp"
#include "__cfws.hpp"

namespace mcs::abnf::imf
{
    /**
     * quoted-string   =   [CFWS]
                           DQUOTE *([FWS] qcontent) [FWS] DQUOTE
                           [CFWS]
     *
     */
    using quoted_string =
        sequence<optional<CFWS>, DQUOTE, zero_or_more<sequence<optional<FWS>, qcontent>>,
                 optional<FWS>, DQUOTE, optional<CFWS>>;
}; // namespace mcs::abnf::imf