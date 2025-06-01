#pragma once

#include "./__field_content.hpp"

namespace mcs::abnf::http
{
    // field-value = *field-content
    using field_value = zero_or_more<field_content>;
}; // namespace mcs::abnf::http