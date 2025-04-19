#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // day = 2DIGIT
    constexpr abnf_result auto day(octet_param_in a, octet_param_in b) noexcept
    {
        return DIGIT(a) && DIGIT(b); // NOLINT
    }
}; // namespace mcs::abnf::http