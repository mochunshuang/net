#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // token68 = 1*( ALPHA / DIGIT / "-" / "." / "_" / "~" / "+" / "/" ) *"="
    constexpr abnf_result auto token68(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);

        std::size_t i{0};
        while (i < k_size &&
               (ALPHA(sp[i]) || DIGIT(sp[i]) || sp[i] == '-' || sp[i] == '.' ||
                sp[i] == '_' || sp[i] == '~' || sp[i] == '+' || sp[i] == '/'))
            ++i;
        if (i == 0)
            return simple_result::fail(0);

        while (i < k_size && sp[i] == '=')
            ++i;
        return i == k_size ? simple_result::success() : simple_result::fail(i);
    }

}; // namespace mcs::abnf::http