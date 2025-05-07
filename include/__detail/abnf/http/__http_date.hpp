#pragma once

#include "./__obs_date.hpp"
#include "./__imf_fixdate.hpp"

namespace mcs::abnf::http
{
    // HTTP-date = IMF-fixdate / obs-date
    using HTTP_date = alternative<IMF_fixdate, obs_date>;

}; // namespace mcs::abnf::http