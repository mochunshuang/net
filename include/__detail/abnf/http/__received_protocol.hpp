#pragma once

#include "./__protocol_name.hpp"
#include "./__protocol_version.hpp"

namespace mcs::abnf::http
{
    // received-protocol = [ protocol-name "/" ] protocol-version
    constexpr abnf_result auto received_protocol(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::fail(0);

        const auto k_ret = protocol_version(sp);
        if (k_ret)
            return builder::success(invalid_span, span{.start = 0, .count = k_size});

        const std::size_t k_index = tool::find_index(sp, '/');
        if (k_index == k_size)
            return builder::fail(0);

        return protocol_name(sp.first(k_index)) &&
                       protocol_version(sp.subspan(k_index + 1))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = k_size - k_index - 1})
                   : builder::fail(k_index);
    }

}; // namespace mcs::abnf::http