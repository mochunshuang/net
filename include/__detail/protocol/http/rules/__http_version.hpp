#pragma once

#include "./__http_name.hpp"

namespace mcs::protocol::http::rules
{
    // HTTP-version = HTTP-name "/" DIGIT "." DIGIT
    using HTTP_version = abnf::sequence<HTTP_name, abnf::Char<'/'>, abnf::DIGIT,
                                        abnf::Char<'.'>, abnf::DIGIT>;
}; // namespace mcs::protocol::http::rules