#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // type = token
    constexpr abnf_result auto type(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http