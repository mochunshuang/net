#pragma once

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::rules
{
    // chunk-data = 1*OCTET
    using chunk_data = abnf::one_or_more<abnf::OCTET>;
}; // namespace mcs::protocol::http::rules