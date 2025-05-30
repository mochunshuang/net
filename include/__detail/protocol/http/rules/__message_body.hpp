#pragma once

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::rules
{
    // message-body = *OCTET
    using message_body = abnf::zero_or_more<abnf::OCTET>;
}; // namespace mcs::protocol::http::rules