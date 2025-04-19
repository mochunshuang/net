#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // subtype = token
    constexpr abnf_result auto subtype(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http