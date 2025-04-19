#pragma once

#include "./__token.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::http
{
    // parameter-value = ( token / quoted-string )
    constexpr abnf_result auto parameter_value(span_param_in sp) noexcept
    {
        return token(sp) || quoted_string(sp);
    }

}; // namespace mcs::abnf::http