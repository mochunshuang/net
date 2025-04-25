#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::operators
{
    struct operate_result
    {
        bool success{false};
        detail::octets_view remaining; // NOLINT
    };

}; // namespace mcs::abnf::operators
