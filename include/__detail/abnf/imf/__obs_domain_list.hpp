#pragma once

#include "./__domain.hpp"

namespace mcs::abnf::imf
{
    /**
     * obs-domain-list =   *(CFWS / ",") "@" domain
     *                     *("," [CFWS] ["@" domain])
     *
     */
    using obs_domain_list =
        sequence<zero_or_more<alternative<CFWS, CharSensitive<','>>>, CharSensitive<'@'>,
                 domain,
                 zero_or_more<sequence<CharSensitive<','>, optional<CFWS>,
                                       optional<sequence<CharSensitive<'@'>, domain>>>>>;
}; // namespace mcs::abnf::imf