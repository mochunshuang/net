#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // range-unit = token
    constexpr abnf_result auto range_unit(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http