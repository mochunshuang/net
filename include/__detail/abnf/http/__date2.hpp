#pragma once

#include "./__day.hpp"
#include "./__month.hpp"

namespace mcs::abnf::http
{

    // date2 = day "-" month "-" 2DIGIT
    constexpr abnf_result auto date2(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 9) // NOLINT:  2+1+3+1+2 ==9
            return false;

        return day(sp[0], sp[1]) && '-' == sp[2] && month(sp.subspan(3, 3)) &&
               '-' == sp[6] && DIGIT(sp[7]) && DIGIT(sp[8]); // NOLINT
    }
}; // namespace mcs::abnf::http