#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::imf
{
    /**
    * @brief  obs-NO-WS-CTL   =   %d1-8 /            ; US-ASCII control
                        %d11 /             ;  characters that do not
                        %d12 /             ;  include the carriage
                        %d14-31 /          ;  return, line feed, and
                        %d127              ;  white space characters
    *
    */
    constexpr abnf_result auto obs_NO_WS_CTL(octet_param_in c) noexcept
    {
        // NOLINTNEXTLINE
        return (c >= 1 && c <= 8) || c == 11 || c == 12 || (c >= 14 && c <= 31) ||
               c == 127; // NOLINT
    }
} // namespace mcs::abnf::imf