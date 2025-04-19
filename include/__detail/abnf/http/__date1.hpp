#pragma once

#include "./__day.hpp"
#include "./__month.hpp"
#include "__year.hpp"

namespace mcs::abnf::http
{
    // year = 4DIGIT
    // day = 2DIGIT
    // date1 = day SP month SP year
    constexpr abnf_result auto date1(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 11) // NOLINT:  2+1+3+1+4 ==11
            return false;

        return day(sp[0], sp[1]) && SP == sp[2] && month(sp.subspan(3, 3)) &&
               SP == sp[6] && year(sp.subspan(7, 4)); // NOLINT
    }
}; // namespace mcs::abnf::http