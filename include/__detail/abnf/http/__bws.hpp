#pragma once

#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // BWS = OWS
    constexpr abnf_result auto BWS(span_param_in sp) noexcept
    {
        return OWS(sp);
    }
}; // namespace mcs::abnf::http