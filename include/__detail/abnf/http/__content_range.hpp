#pragma once

#include "./__range_unit.hpp"
#include "./__range_resp.hpp"
#include "./__unsatisfied_range.hpp"

namespace mcs::abnf::http
{
    // Content-Range = range-unit SP ( range-resp / unsatisfied-range )
    using Content_Range =
        sequence<range_unit, SP, alternative<range_resp, unsatisfied_range>>; // NOLINT
}; // namespace mcs::abnf::http