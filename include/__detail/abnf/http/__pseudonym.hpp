#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // pseudonym = token
    constexpr abnf_result auto pseudonym(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http