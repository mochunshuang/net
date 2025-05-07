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
        sequence<zero_or_more<alternative<CFWS, Char<','>>>, Char<'@'>, domain,
                 zero_or_more<sequence<Char<','>, optional<CFWS>,
                                       optional<sequence<Char<'@'>, domain>>>>>;
}; // namespace mcs::abnf::imf