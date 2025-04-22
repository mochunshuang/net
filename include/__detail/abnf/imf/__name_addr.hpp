#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::imf
{
    // name-addr       =   [display-name] angle-addr
    constexpr abnf_result auto name_addr(span_param_in sp) noexcept
    {
        return true;
    }
} // namespace mcs::abnf::imf