#pragma once

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::rules
{
    // chunk-size = 1*HEXDIG
    using chunk_size = abnf::one_or_more<abnf::HEXDIG>;
}; // namespace mcs::protocol::http::rules