#pragma once

#include "../../../abnf/http/__transfer_coding.hpp"

namespace mcs::protocol::http::rules
{
    // transfer-coding = <transfer-coding, see [HTTP], Section 10.1.4>
    using transfer_coding = abnf::http::transfer_coding;
}; // namespace mcs::protocol::http::rules