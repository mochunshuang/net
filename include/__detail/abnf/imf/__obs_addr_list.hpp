#pragma once

#include "./__cfws.hpp"
#include "__address.hpp"

namespace mcs::abnf::imf
{
    //  obs-addr-list   =   *([CFWS] ",") address *("," [address / CFWS])
    using obs_addr_list = sequence<
        zero_or_more<sequence<optional<CFWS>, CharSensitive<','>>>, address,
        zero_or_more<sequence<CharSensitive<','>, optional<alternative<address, CFWS>>>>>;
}; // namespace mcs::abnf::imf