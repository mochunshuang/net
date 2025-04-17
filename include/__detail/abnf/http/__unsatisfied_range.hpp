#pragma once

#include "./__complete_length.hpp"

namespace mcs::abnf::http
{
    // unsatisfied-range = "*/" complete-length
    constexpr abnf_result auto unsatisfied_range(span_param_in sp) noexcept
    {
        return sp.size() == 3 && sp[0] == '*' && sp[1] == '/' &&
               complete_length(sp.subspan(2 + 1));
    }

}; // namespace mcs::abnf::http