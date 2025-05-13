#pragma once

#include "./__start_line.hpp"
#include "./__field_line.hpp"
#include "./__message_body.hpp"

namespace mcs::protocol::http::rules
{
    // HTTP-message = start-line CRLF *( field-line CRLF ) CRLF [ message-body ]
    using HTTP_message =
        abnf::sequence<start_line, abnf::CRLF,
                       abnf::zero_or_more<abnf::sequence<field_line, abnf::CRLF>>,
                       abnf::CRLF, abnf::optional<message_body>>;
}; // namespace mcs::protocol::http::rules