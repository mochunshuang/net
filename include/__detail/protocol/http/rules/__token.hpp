#pragma once

#include "../../../abnf/http/__token.hpp"

namespace mcs::protocol::http::rules
{
    // token = <token, see [HTTP], Section 5.6.2>
    using token = ::mcs::abnf::http::token;
}; // namespace mcs::protocol::http::rules