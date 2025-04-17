#pragma once

#include "../__core_rules.hpp"
#include <cstddef>

namespace mcs::abnf::http
{
    // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
    constexpr abnf_result auto qvalue(span_param_in sp) noexcept
    {
        if (sp.empty())
            return false;
        const auto k_sub_sp = sp.subspan(1);
        const auto k_size = k_sub_sp.size();

        if (sp[0] == '0') // "0" [ "." *3DIGIT ]
        {
            if (k_size == 0)
                return true;
            if (k_size > 4 || k_sub_sp[0] != '.')
                return false;
            for (std::size_t i{1}; i < k_size; ++i)
            {
                if (not DIGIT(k_sub_sp[i]))
                    return false;
            }
            return true;
        }

        // "1" [ "." *3"0" ]
        if (sp[0] != '1')
            return false;
        if (k_size == 0)
            return true;
        if (k_size > 4 || k_sub_sp[0] != '.')
            return false;
        for (std::size_t i{1}; i < k_size; ++i)
        {
            if (k_sub_sp[i] != '0')
                return false;
        }
        return true;
    }
}; // namespace mcs::abnf::http