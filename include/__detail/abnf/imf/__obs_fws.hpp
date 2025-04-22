#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::imf
{
    // obs-FWS         =   1*WSP *(CRLF 1*WSP)
    constexpr abnf_result auto obs_FWS(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 1)
            return simple_result::fail(0);

        std::size_t i = 0;
        while (i < k_size && WSP(sp[i])) // NOTE: 1*WSP
            ++i;
        if (i == 0)
            return simple_result::fail(i);

        static_assert(not WSP(CR));
        while (i < k_size)
        {
            if (sp[i] != CR) // CRLF
                return simple_result::fail(i);
            ++i;

            if (i == k_size || sp[i] != LF) // CRLF
                return simple_result::fail(i);
            ++i;

            if (i == k_size || not WSP(sp[i])) // WSP
                return simple_result::fail(i);
            ++i;

            while (i < k_size && WSP(sp[i]))
                ++i;
        }

        return simple_result::success();
    }
} // namespace mcs::abnf::imf