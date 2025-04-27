#pragma once

#include "./__gen_delims.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    // reserved      = gen-delims / sub-delims
    using reserved = alternative<gen_delims, sub_delims>;
}; // namespace mcs::abnf::uri