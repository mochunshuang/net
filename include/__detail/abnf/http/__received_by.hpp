#pragma once

#include "./__pseudonym.hpp"
#include "./__port.hpp"

namespace mcs::abnf::http
{
    // received-by = pseudonym [ ":" port ]
    using received_by = sequence<pseudonym, optional<sequence<Char<':'>, port>>>;

}; // namespace mcs::abnf::http