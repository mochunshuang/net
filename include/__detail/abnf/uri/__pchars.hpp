#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{

    // pchars         = *pchar
    constexpr auto pchars(span_param_in sp) noexcept -> abnf_result auto
    {
        const auto k_size = sp.size();
        using builder = result_builder<result<1>>;
        size_t index = 0;
        while (index < k_size)
        {
            const auto &c = sp[index];

            // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
            static_assert(not pchar('%'));
            static_assert(not unreserved('%'));
            if (c == '%')
            {
                if (index + 2 < k_size) // check: if match "%41"
                {
                    if (pchar(c, sp[index + 1], sp[index + 2]))
                    {
                        index += 3;
                        continue;
                    }
                }
            }
            else
            {
                if (pchar(c))
                {
                    index++;
                    continue;
                }
            }
            return builder::fail(index);
        }
        return builder::success(span{.start = 0, .count = k_size});
    }
}; // namespace mcs::abnf::uri