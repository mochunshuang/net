#pragma once

#include "./__reg_name.hpp"

namespace mcs::abnf::uri
{
    // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    constexpr auto userinfo(span_param_in sp) noexcept -> abnf_result auto
    {
        using builder = result_builder<result<1>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::fail(0);

        // handle: 1*( unreserved / pct-encoded / sub-delims / ":" )
        // NOTE: reg-name = *( unreserved / pct-encoded / sub-delims )
        size_t index = 0;
        while (index < k_size)
        {
            const auto k_span = sp.subspan(index);
            const auto k_ret = reg_name(k_span);
            if (k_ret)
            {
                index += k_span.size();
                continue;
            }

            static_assert(not reg_name(std::array<OCTET, 1>{':'}));
            index = index + k_ret.error().index();
            if (sp[index] == ':')
            {
                ++index;
                continue;
            }
            return builder::fail(index);
        }
        return builder::success(span{.start = 0, .count = k_size});
    }
}; // namespace mcs::abnf::uri