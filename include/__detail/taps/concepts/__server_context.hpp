#pragma once

#include <concepts>

namespace mcs::taps::concepts
{

    template <typename Context>
    concept server_context = requires(Context &ctx) {
        { ctx.Listen() } -> std::same_as<void>;
    };

}; // namespace mcs::taps::concepts
