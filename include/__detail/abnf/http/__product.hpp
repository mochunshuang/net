#pragma once

#include "./__token.hpp"
#include "./__protocol_version.hpp"

namespace mcs::abnf::http
{
    // product = token [ "/" product-version ]
    using product = sequence<token, optional<sequence<Char<'/'>, protocol_version>>>;
}; // namespace mcs::abnf::http