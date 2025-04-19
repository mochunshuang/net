#pragma once

#include "../__core_rules.hpp"
#include "./__month.hpp"

namespace mcs::abnf::http
{

    // month SP ( 2DIGIT / ( SP DIGIT ) )
    constexpr abnf_result auto date2(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size != 6) // NOLINT:  3+1+2==6
            return false;

        return month(sp[0], sp[1], sp[2]) && sp[3] == SP && // NOLINTNEXTLINE
               ((DIGIT(sp[4]) && DIGIT(sp[5])) || (sp[4] == SP && DIGIT(sp[5])));
    }
}; // namespace mcs::abnf::http
