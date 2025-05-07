#pragma once

#include "./__comment.hpp"

namespace mcs::abnf::imf
{
    // CFWS            =   (1*([FWS] comment) [FWS]) / FWS
    using CFWS = alternative<
        sequence<one_or_more<sequence<optional<FWS>, comment>>, optional<FWS>>, FWS>;
}; // namespace mcs::abnf::imf