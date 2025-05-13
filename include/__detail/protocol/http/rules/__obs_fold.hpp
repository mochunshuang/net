#pragma once

#include "./__ows.hpp"
#include "./__rws.hpp"

namespace mcs::protocol::http::rules
{
    // obs-fold = OWS CRLF RWS
    using obs_fold = abnf::sequence<OWS, abnf::CRLF, RWS>;
}; // namespace mcs::protocol::http::rules