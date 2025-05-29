#pragma once

#include <concepts>

namespace mcs::taps::concepts
{

    template <typename Context>
    concept client_context = requires(Context &ctx) {
        { ctx.Initiate() } -> std::same_as<void>;
    };

}; // namespace mcs::taps::concepts
