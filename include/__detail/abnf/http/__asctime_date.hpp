#pragma once

#include "./__day_name.hpp"
#include "./__date3.hpp"
#include "./__time_of_day.hpp"
#include "./__year.hpp"

namespace mcs::abnf::http
{
    // asctime-date = day-name SP date3 SP time-of-day SP year
    using asctime_date = sequence<day_name, SP, date3, SP, time_of_day, SP, year>;
}; // namespace mcs::abnf::http