#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct query
    {
    };

    constexpr ReturnType<query> make_query(span_param_in sp) noexcept
    {
        if (parse::query(sp))
            return query{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri