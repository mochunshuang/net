#pragma once

#include "./__delay_seconds.hpp"
#include "./__http_date.hpp"

namespace mcs::abnf::http
{
    // Retry-After = HTTP-date / delay-seconds
    using Retry_After = alternative<HTTP_date, delay_seconds>;
}; // namespace mcs::abnf::http