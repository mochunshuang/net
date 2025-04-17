#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // year = 4DIGIT
    constexpr abnf_result auto RWS(span_param_in sp) noexcept
    {
        return sp.size() == 4 && DIGIT(sp[0]) && DIGIT(sp[1]) && DIGIT(sp[2]) &&
               DIGIT(sp[3]);
    }
}; // namespace mcs::abnf::http