#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::uri
{
    /**
     * @brief
     * dec-octet = DIGIT                 ; 0-9
     *           / %x31-39 DIGIT         ; 10-99
     *           / "1" 2DIGIT            ; 100-199
     *           / "2" %x30-34 DIGIT     ; 200-249
     *           / "25" %x30-35          ; 250-255
     */
    constexpr CheckResult dec_octet(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();

        if (k_size == 1 && DIGIT(sp[0]))
            return Success{k_size};
        if (k_size == 2 && sp[0] >= '1' && sp[0] <= '9' && DIGIT(sp[1]))
            return Success{k_size};

        if (k_size == 3)
        {
            if (sp[0] == '1' && DIGIT(sp[1]) && DIGIT(sp[2]))
                return Success{k_size};
            if (sp[0] == '2' && sp[1] >= '0' && sp[1] <= '4' && DIGIT(sp[2]))
                return Success{k_size};
            if (sp[0] == '2' && sp[1] == '5' && sp[2] >= '0' && sp[2] <= '5')
                return Success{k_size};
        }
        return Fail(0);
    }
}; // namespace mcs::abnf::uri