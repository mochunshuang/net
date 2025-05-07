#pragma once

#include "./__token.hpp"
#include "./__ows.hpp"
#include "./__transfer_parameter.hpp"

namespace mcs::abnf::http
{
    // transfer-coding = token *( OWS ";" OWS transfer-parameter )
    using transfer_coding =
        sequence<token, zero_or_more<sequence<OWS, Char<';'>, OWS, transfer_parameter>>>;
}; // namespace mcs::abnf::http