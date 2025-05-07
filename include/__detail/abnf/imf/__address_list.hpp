#pragma once

#include "./__address.hpp"
#include "./__obs_addr_list.hpp"

namespace mcs::abnf::imf
{
    // address-list    =   (address *("," address)) / obs-addr-list
    using address_list = alternative<
        sequence<address, zero_or_more<sequence<CharSensitive<','>, address>>>,
        obs_addr_list>;

}; // namespace mcs::abnf::imf