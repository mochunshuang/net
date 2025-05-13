#pragma once

#include "./__http_version.hpp"
#include "./__status_code.hpp"
#include "./__reason_phrase.hpp"

namespace mcs::protocol::http::rules
{
    // status-line = HTTP-version SP status-code SP [ reason-phrase ]
    using status_line = abnf::sequence<HTTP_version, abnf::SP, status_code, abnf::SP,
                                       abnf::optional<reason_phrase>>;
}; // namespace mcs::protocol::http::rules