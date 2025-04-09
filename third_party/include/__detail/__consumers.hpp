#pragma once
#include "./consumers/__sync_wait_with_variant.hpp"

namespace mcs::this_thread
{
    using mcs::execution::consumers::__sync_wait::sync_wait_t;
    using mcs::execution::consumers::sync_wait; // NOLINT

    using mcs::execution::consumers::__sync_wait::sync_wait_with_variant_t;
    using mcs::execution::consumers::sync_wait_with_variant; // NOLINT
}; // namespace mcs::this_thread