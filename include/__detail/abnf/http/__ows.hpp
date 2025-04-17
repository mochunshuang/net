#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::http
{
    // OWS = *( SP / HTAB )
    constexpr abnf_result auto OWS(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        for (std::size_t i{0}; i < k_size; ++i)
        {
            if (sp[i] != SP && sp[i] != HTAB)
                return simple_result::fail(i);
        }
        return simple_result::success();
    }
}; // namespace mcs::abnf::http