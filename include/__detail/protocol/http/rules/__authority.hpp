#pragma once

#include "../../../abnf/http/__authority.hpp"

namespace mcs::protocol::http::rules
{
    // authority = <authority, see [URI], Section 3.2>
    using authority = abnf::http::authority;
}; // namespace mcs::protocol::http::rules