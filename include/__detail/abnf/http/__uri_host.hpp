#pragma once

#include "../uri/__host.hpp"

namespace mcs::abnf::http
{
    // uri-host = <host, see [URI], Section 3.2.2>
    constexpr abnf_result auto uri_host(span_param_in sp) noexcept
    {
        return uri::host(sp);
    }
}; // namespace mcs::abnf::http