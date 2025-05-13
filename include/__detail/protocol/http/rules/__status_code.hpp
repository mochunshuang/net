#pragma once

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::rules
{
    // status-code = 3DIGIT
    using status_code = abnf::times<3, abnf::DIGIT>;
}; // namespace mcs::protocol::http::rules