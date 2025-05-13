#pragma once

#include "./__transfer_coding.hpp"
#include "./__ows.hpp"

namespace mcs::protocol::http::rules
{
    // Transfer-Encoding = [ transfer-coding *( OWS "," OWS transfer-coding ) ]
    using Transfer_Encoding = abnf::optional<abnf::sequence<
        transfer_coding,
        abnf::zero_or_more<abnf::sequence<OWS, abnf::Char<','>, OWS, transfer_coding>>>>;
}; // namespace mcs::protocol::http::rules