#pragma once

#include "./__day_name_l.hpp"
#include "./__date2.hpp"
#include "./__time_of_day.hpp"
#include "./__gmt.hpp"

namespace mcs::abnf::http
{
    // rfc850-date = day-name-l "," SP date2 SP time-of-day SP GMT
    constexpr abnf_result auto rfc850_date(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        std::size_t index = 0;
        while (index < k_size && sp[index] != ',')
            ++index;
        if (index == k_size)
            return false;

        constexpr auto k_day_name_l_min = 6;
        constexpr auto k_day_name_l_max = 9;
        if (index < k_day_name_l_min || index > k_day_name_l_max ||
            not day_name_l(sp.first(index)))
            return false;

        if (index + 23 >= k_size) // 1 + 9 + 1 + 8 +1 + 3 // NOLINT
            return false;

        return sp[index + 1] == SP && date2(sp.subspan(index + 2, 9)) &&         // NOLINT
               sp[index + 11] == SP && time_of_day(sp.subspan(index + 12, 8)) && // NOLINT
               sp[index + 20] == SP &&                                           // NOLINT
               GMT(sp[index + 21], sp[index + 22], sp[index + 23]);              // NOLINT
    }
}; // namespace mcs::abnf::http