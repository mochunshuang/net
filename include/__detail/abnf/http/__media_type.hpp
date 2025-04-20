#pragma once

#include "./__type.hpp"
#include "./__subtype.hpp"
#include "./__parameters.hpp"
#include "__tchar.hpp"

namespace mcs::abnf::http
{
    // subtype = token
    // media-type = type "/" subtype parameters
    constexpr abnf_result auto media_type(span_param_in sp) noexcept
    {
        using builder = result_builder<result<3>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::fail(0);
        static_assert(not tchar('/'));
        auto const k_index = tool::find_index(sp, '/');

        if (k_index == k_size || not type(sp.first(k_index))) // no '/'
            return builder::fail(k_index);

        // check subtype
        auto ret = subtype(sp.subspan(k_index + 1));

        if (ret)
            return builder::success(
                span{.start = 0, .count = k_index},
                span{.start = k_index + 1, .count = k_size - k_index - 1}, invalid_span);

        auto begin = k_index + 1 + ret.error().index();
        return ret.error().index() != 0 && parameters(sp.subspan(begin))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = begin - k_index - 1},
                         span{.start = begin, .count = k_size - begin})
                   : builder::fail(ret.error().index());
    }
}; // namespace mcs::abnf::http
