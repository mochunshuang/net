#pragma once

#include "./__token.hpp"

namespace mcs::abnf::http
{
    // protocol-version = token
    constexpr abnf_result auto protocol_version(span_param_in sp) noexcept
    {
        return token(sp);
    }

}; // namespace mcs::abnf::http