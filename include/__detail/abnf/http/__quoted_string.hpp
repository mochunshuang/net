#pragma once

#include "../__core_rules.hpp"
#include "./__qdtext.hpp"
#include "./__quoted_pair.hpp"

namespace mcs::abnf::http
{
    // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
    constexpr abnf_result auto quoted_string(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size < 2)
            return simple_result::fail(0);

        if (sp[0] != DQUOTE && sp[k_size - 1] != DQUOTE)
            return simple_result::fail(0);

        for (std::size_t i{1}, end = k_size - 1; i < end;)
        {
            if (qdtext(sp[i]))
            {
                ++i;
                continue;
            }
            static_assert(not qdtext('\\'));
            if (i + 1 < end && quoted_pair(sp[i], sp[i + 1]))
            {
                i += 2;
                continue;
            }
            return simple_result::fail(i);
        }
        return simple_result::success();
    }
}; // namespace mcs::abnf::http