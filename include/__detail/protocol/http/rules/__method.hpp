#pragma once

#include "../../../abnf/http/__method.hpp"

namespace mcs::protocol::http::rules
{
    // method = token
    using method = ::mcs::abnf::http::method;
}; // namespace mcs::protocol::http::rules