#pragma once

#include "./__type.hpp"
#include "./__subtype.hpp"
#include "./__parameters.hpp"
#include "__parameter.hpp"
#include "__tchar.hpp"

namespace mcs::abnf::http
{
    // subtype = token
    // media-range = ( "*/*" / ( type "/*" ) / ( type "/" subtype ) ) parameters
    constexpr abnf_result auto media_range(span_param_in sp) noexcept
    {
        using builder = result_builder<result<3>>;
        const auto k_size = sp.size();
        if (k_size < 3)
            return builder::fail(0);

        static_assert(not tchar('/'));
        static_assert(tchar('*'));

        if (sp[0] == '*' && sp[1] == '/' && sp[2] == '*') // NOTE: case 0
            return parameters(sp.subspan(3))
                       ? builder::success(span{.start = 0, .count = 1},
                                          span{.start = 2, .count = 1},
                                          span{.start = 3, .count = k_size - 3})
                       : builder::fail(3);

        auto const k_index = tool::find_index(sp, '/');
        if (k_index == k_size || not type(sp.first(k_index))) // check type
            return builder::fail(k_index);

        if (sp[k_index + 1] == '*') // NOTE: case 1
            return parameters(sp.subspan(k_index + 2))
                       ? builder::success(
                             span{.start = 0, .count = k_index},
                             span{.start = k_index + 1, .count = 1},
                             span{.start = k_index + 2, .count = k_size - k_index - 2})
                       : builder::fail(k_index);

        // check subtype
        auto ret = subtype(sp.subspan(k_index + 1));
        if (ret) // NOTE: case 2.1: no parameters
            return builder::success(
                span{.start = 0, .count = k_index},
                span{.start = k_index + 1, .count = k_size - k_index - 1}, invalid_span);

        auto begin = k_index + 1 + ret.error().index(); // NOTE: case 2.2
        return ret.error().index() != 0 && parameters(sp.subspan(begin))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = begin - k_index - 1},
                         span{.start = begin, .count = k_size - begin})
                   : builder::fail(ret.error().index());
    }
}; // namespace mcs::abnf::http
