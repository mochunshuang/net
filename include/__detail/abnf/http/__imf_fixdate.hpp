#pragma once

#include "./__day_name.hpp"
#include "./__date1.hpp"
#include "./__time_of_day.hpp"
#include "./__gmt.hpp"

namespace mcs::abnf::http
{
    // IMF-fixdate = day-name "," SP date1 SP time-of-day SP GMT
    using IMF_fixdate =
        sequence<day_name, Char<','>, SP, date1, SP, time_of_day, SP, GMT>;
}; // namespace mcs::abnf::http