#pragma once

#include "./iana/__method_registration.hpp"

namespace mcs::protocol::http
{
    // request-line = method SP request-target SP HTTP-version
    struct RequestLine
    {
        iana::MethodRegistration::Method method;
    };
}; // namespace mcs::protocol::http