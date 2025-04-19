#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // parameter-name = token
    constexpr abnf_result auto parameter_name(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http