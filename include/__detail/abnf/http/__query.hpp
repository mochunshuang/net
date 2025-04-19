#pragma once

#include "../uri/__query.hpp"

namespace mcs::abnf::http
{
    // query = <query, see [URI], Section 3.4>
    constexpr abnf_result auto query(span_param_in sp) noexcept
    {
        return uri::query(sp);
    }
}; // namespace mcs::abnf::http