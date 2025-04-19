#pragma once

#include "./__parameter.hpp"

namespace mcs::abnf::http
{
    // OWS = *( SP / HTAB )
    // parameters = *( OWS ";" OWS [ parameter ] )
    constexpr abnf_result auto parameters(span_param_in sp) noexcept
    {
        std::size_t index = 0;
        const auto k_len = sp.size();
        while (index < k_len)
        {
            // NOTE: 1. skip OWS
            while (index < k_len && (sp[index] == SP || sp[index] == HTAB))
                ++index;

            if (sp[index] == ';') // NOTE: skip ';'
                ++index;
            else
                return simple_result::fail(index);

            // NOTE: 3. skip OWS
            while (index < k_len && (sp[index] == SP || sp[index] == HTAB))
                ++index;

            if (index == k_len) // NOTE: empty  parameter no need check
                break;

            // find next OWS or next ';' to fix parameter span
            auto next_start = k_len;
            for (auto i = index; i < k_len; ++i)
            {
                if ((sp[i] == SP || sp[i] == HTAB || sp[i] == ';'))
                {
                    next_start = i;
                    break;
                }
            }
            if (next_start == k_len)
            {
                if (auto const k_check_span = sp.subspan(index); parameter(k_check_span))
                    index += k_check_span.size();
            }
            else if (next_start - index > 2) // t = v ==3
            {
                // NOTE: 4. parameter
                if (auto const k_check_span = sp.subspan(index, next_start - index);
                    parameter(k_check_span))
                    index = next_start;
            }

            static_assert(true);
        }

        return (index == k_len) ? simple_result::success() : simple_result::fail(index);
    }

}; // namespace mcs::abnf::http