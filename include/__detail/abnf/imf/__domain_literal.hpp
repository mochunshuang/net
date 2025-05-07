#pragma once

#include "./__cfws.hpp"
#include "./__dtext.hpp"

namespace mcs::abnf::imf
{
    // domain-literal  =   [CFWS] "[" *([FWS] dtext) [FWS] "]" [CFWS]
    using domain_literal =
        sequence<optional<CFWS>, Char<'['>, zero_or_more<sequence<optional<FWS>, dtext>>,
                 optional<FWS>, Char<']'>, optional<CFWS>>;
}; // namespace mcs::abnf::imf