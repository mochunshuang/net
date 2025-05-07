#pragma once

#include "./__langtag.hpp"
#include "./__privateuse.hpp"
#include "./__grandfathered.hpp"

namespace mcs::abnf::tfil
{
    /**
     * Language-Tag  = langtag             ; normal language tags
                   / privateuse          ; private use tag
                   / grandfathered       ; grandfathered tags
     *
     */
    using Language_Tag = alternative<langtag, privateuse, grandfathered>;

}; // namespace mcs::abnf::tfil