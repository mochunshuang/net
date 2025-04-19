#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // protocol-name = token
    constexpr abnf_result auto protocol_name(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http