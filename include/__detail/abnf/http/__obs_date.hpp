#pragma once

#include "./__rfc850_date.hpp"
#include "./__asctime_date.hpp"

namespace mcs::abnf::http
{
    // obs-date = rfc850-date / asctime-date
    using obs_date = alternative<rfc850_date, asctime_date>;
}; // namespace mcs::abnf::http