#pragma once

#include "./__day.hpp"
#include "./__month.hpp"
#include "./__year.hpp"

namespace mcs::abnf::http
{
    // date1 = day SP month SP year
    using date1 = sequence<day, SP, month, SP, year>;
}; // namespace mcs::abnf::http