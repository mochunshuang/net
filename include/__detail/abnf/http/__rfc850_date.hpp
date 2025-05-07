#pragma once

#include "./__day_name_l.hpp"
#include "./__date2.hpp"
#include "./__time_of_day.hpp"
#include "./__gmt.hpp"

namespace mcs::abnf::http
{
    // rfc850-date = day-name-l "," SP date2 SP time-of-day SP GMT
    using rfc850_date =
        sequence<day_name_l, Char<','>, SP, date2, SP, time_of_day, SP, GMT>;
}; // namespace mcs::abnf::http