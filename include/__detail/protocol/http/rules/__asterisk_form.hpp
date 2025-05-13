#pragma once

#include "../../../abnf/__abnf.hpp"

namespace mcs::protocol::http::rules
{
    // asterisk-form = "*"
    using asterisk_form = abnf::Char<'*'>;
}; // namespace mcs::protocol::http::rules