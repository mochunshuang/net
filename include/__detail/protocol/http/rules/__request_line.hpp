#pragma once

#include "./__method.hpp"
#include "./__request_target.hpp"
#include "./__http_version.hpp"

namespace mcs::protocol::http::rules
{
    // request-line = method SP request-target SP HTTP-version
    using request_line =
        abnf::sequence<method, abnf::SP, request_target, abnf::SP, HTTP_version>;
}; // namespace mcs::protocol::http::rules