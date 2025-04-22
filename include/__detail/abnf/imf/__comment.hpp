#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::imf
{
    // comment         =   "(" *([FWS] ccontent) [FWS] ")"
    constexpr abnf_result auto comment(span_param_in sp) noexcept;
    
} // namespace mcs::abnf::imf