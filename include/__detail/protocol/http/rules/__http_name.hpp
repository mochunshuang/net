#pragma once

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::rules
{
    // HTTP-name = %x48.54.54.50 ; HTTP
    using HTTP_name = abnf::StringSensitive<"HTTP">;
}; // namespace mcs::protocol::http::rules