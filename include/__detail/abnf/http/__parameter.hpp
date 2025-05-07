#pragma once

#include "./__parameter_name.hpp"
#include "./__parameter_value.hpp"

namespace mcs::abnf::http
{
    // parameter = parameter-name "=" parameter-value
    using parameter = sequence<parameter_name, Char<'='>, parameter_value>;

}; // namespace mcs::abnf::http