#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
    using sub_delims = any_of<'!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='>;
}; // namespace mcs::abnf::uri