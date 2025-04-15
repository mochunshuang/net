#pragma once

#include "../abnf/__core_types.hpp"
#include "../abnf/uri_abnf.hpp"

namespace mcs::rfc
{
    using abnf::span_param_in;
    using abnf::octet_param_in;
    using abnf::empty_span_param;

    namespace parse = mcs::abnf::uri; // NOLINT

}; // namespace mcs::rfc