#pragma once

#include "./__opaque_tag.hpp"
#include "./__weak.hpp"

namespace mcs::abnf::http
{
    // entity-tag = [ weak ] opaque-tag
    using entity_tag = sequence<optional<weak>, opaque_tag>;
}; // namespace mcs::abnf::http