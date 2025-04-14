#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    //  reg-name      = *( unreserved / pct-encoded / sub-delims )
    constexpr auto reg_name(span_param_in sp) noexcept -> abnf_result auto
    {
        using builder = result_builder<result<1>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::success(span{.start = 0, .count = k_size});

        size_t index = 0;
        while (index < k_size)
        {
            const auto &c = sp[index];
            if (unreserved(c)) // 1、unreserved
            {
                ++index;
                continue;
            }

            static_assert(not sub_delims('%'));
            if (c == '%') // 2、pct-encoded   = "%" HEXDIG HEXDIG
            {
                if (index + 2 < k_size)
                {
                    if (pct_encoded(c, sp[index + 1], sp[index + 2]))
                    {
                        index += 3;
                        continue;
                    }
                }
            }
            else // 3、sub-delims
            {
                if (sub_delims(c))
                {
                    ++index;
                    continue;
                }
            }
            return builder::fail(index);
        }
        return builder::success(span{.start = 0, .count = k_size});
    }
}; // namespace mcs::abnf::uri