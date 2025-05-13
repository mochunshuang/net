#pragma once

#include "../../../abnf/http/__field_name.hpp"

namespace mcs::protocol::http::rules
{
    // field-name = <field-name, see [HTTP], Section 5.1>
    using field_name = abnf::http::field_name;
}; // namespace mcs::protocol::http::rules