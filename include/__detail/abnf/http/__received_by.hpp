#pragma once

#include "./__pseudonym.hpp"
#include "./__port.hpp"

namespace mcs::abnf::http
{
    // received-by = pseudonym [ ":" port ]
    constexpr abnf_result auto received_by(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::fail(0);

        const auto k_ret = pseudonym(sp);
        if (k_ret)
            return builder::success(span{.start = 0, .count = k_size}, invalid_span);

        const std::size_t k_index = tool::find_index(sp, ':');
        if (k_index == k_size)
            return builder::fail(0);

        return pseudonym(sp.first(k_index)) && port(sp.subspan(k_index + 1))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = k_size - k_index - 1})
                   : builder::fail(k_index);
    }

}; // namespace mcs::abnf::http