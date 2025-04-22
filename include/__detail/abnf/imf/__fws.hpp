#pragma once

#include "../__core_rules.hpp"
#include "./__obs_fws.hpp"

namespace mcs::abnf::imf
{
    // FWS             =   ([*WSP CRLF] 1*WSP) /  obs-FWS ;Folding white space
    constexpr abnf_result auto FWS(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 1)
            return simple_result::fail(0);

        std::size_t i = 0;
        while (i < k_size && WSP(sp[i])) // NOTE: [*WSP]
            ++i;
        if (i == k_size) // NOTE: match 1 and only  1*WSP
            return simple_result::success();

        if (i + 2 < k_size && sp[i] == CR && sp[i + 1] == LF) // NOTE: [CRLF]
            i += 2;

        while (i < k_size && WSP(sp[i])) // NOTE: 1*WSP
            ++i;
        if (i == k_size)
            return simple_result::success();

        return obs_FWS(sp);
    }
} // namespace mcs::abnf::imf