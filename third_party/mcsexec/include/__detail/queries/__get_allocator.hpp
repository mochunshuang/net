#pragma once

#include "../__core_concepts.hpp"
#include "./__forwarding_query.hpp"

namespace mcs::execution::queries
{
    struct get_allocator_t : forwarding_query_t
    {
        using __self_t = get_allocator_t;

        // get_allocator(env) is expression-equivalent to
        // MANDATE-NOTHROW(as_const(env).query(get_allocator)).
        template <typename T>
            requires requires(T &&env) {
                { std::as_const(env).query(std::declval<__self_t>()) } noexcept;
            }
        constexpr auto operator()(const T &env) const noexcept -> simple_allocator auto
        {
            return env.query(*this);
        }
    };
    inline constexpr get_allocator_t get_allocator{}; // NOLINT

}; // namespace mcs::execution::queries