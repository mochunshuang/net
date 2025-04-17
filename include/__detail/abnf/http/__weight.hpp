#pragma once

#include "./__qvalue.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // OWS = *( SP / HTAB )
    // weight = OWS ";" OWS "q=" qvalue
    constexpr abnf_result auto weight(span_param_in sp) noexcept
    {
        using builder = result_builder<result<1>>;
        static_assert(';' != SP && ';' != HTAB);

        const auto k_size = sp.size();
        if (k_size < 3)
            return builder::fail(0);

        std::size_t idx_0 = k_size;
        std::size_t idx_1 = k_size;
        for (std::size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == ';')
            {
                idx_0 = i;
                break;
            }
        }
        if (idx_0 == k_size)
            return builder::fail(0);
        for (std::size_t i = idx_0 + 1; i < k_size; ++i)
        {
            if (sp[i] == 'q')
            {
                idx_1 = i;
                break;
            }
        }
        if (idx_1 == k_size || idx_1 + 1 == k_size || sp[idx_1 + 1] != '=')
            return builder::fail(0);
        // weight = OWS ";" OWS "q=" qvalue
        const auto k_ows0 = sp.first(idx_0);
        const auto k_ows1 = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        const auto k_qvalue = sp.subspan(idx_1 + 2, (k_size - 1) - idx_1 - 1);
        if (not(OWS(k_ows0) && OWS(k_ows1) && qvalue(k_qvalue)))
            return builder::fail(0);

        return builder::success(
            span{.start = idx_1 + 2, .count = (k_size - 1) - idx_1 - 1});
    }
}; // namespace mcs::abnf::http