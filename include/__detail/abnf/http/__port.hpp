#pragma once

#include "../uri/__port.hpp"

namespace mcs::abnf::http
{
    // port = <port, see [URI], Section 3.2.3>
    constexpr abnf_result auto port(span_param_in sp) noexcept
    {
        return uri::port(sp);
    }
}; // namespace mcs::abnf::http
