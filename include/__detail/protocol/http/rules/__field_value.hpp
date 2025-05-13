#pragma once

#include "../../../abnf/http/__field_value.hpp"

namespace mcs::protocol::http::rules
{
    // field-value = <field-value, see [HTTP], Section 5.5>
    using field_value = ::mcs::abnf::http::field_value;
}; // namespace mcs::protocol::http::rules