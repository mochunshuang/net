#pragma once

#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
    constexpr abnf_result auto transfer_parameter(span_param_in sp) noexcept
    {
        return OWS(sp);
    }
}; // namespace mcs::abnf::http