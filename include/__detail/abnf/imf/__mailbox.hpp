#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::imf
{
    // mailbox         =   name-addr / addr-spec
    constexpr abnf_result auto mailbox(span_param_in sp) noexcept
    {
        return true;
    }
} // namespace mcs::abnf::imf