#pragma once

#include "./__pchars.hpp"

namespace mcs::abnf::uri
{
    // segment-nz    = 1*pchar
    constexpr auto segment_nz(span_param_in sp) noexcept -> abnf_result auto
    {
        using builder = result_builder<result<1>>;
        if (sp.empty())
            return builder::fail(0);
        return pchars(sp);
    }
}; // namespace mcs::abnf::uri