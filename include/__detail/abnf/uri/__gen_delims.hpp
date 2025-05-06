#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    //  gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
    using gen_delims = any_of<':', '/', '?', '#', '[', ']', '@'>;
}; // namespace mcs::abnf::uri