#pragma once

#include "./__expectation.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // Expect = [ expectation *( OWS "," OWS expectation ) ]
    using Expect = optional<
        sequence<expectation, zero_or_more<sequence<OWS, Char<','>, OWS, expectation>>>>;
}; // namespace mcs::abnf::http