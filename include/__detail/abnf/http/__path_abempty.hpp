#pragma once

#include "../uri/__path_abempty.hpp"

namespace mcs::abnf::http
{
    // path-abempty = <path-abempty, see [URI], Section 3.3>
    constexpr abnf_result auto path_abempty(span_param_in sp) noexcept
    {
        return uri::path_abempty(sp);
    }
}; // namespace mcs::abnf::http
