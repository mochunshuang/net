#pragma once

#include "./__mutable_buffer.hpp"
#include "./__const_buffer.hpp"

namespace mcs::net::io::buffer
{
    // Returns: std::addressof(b).
    constexpr const auto *buffer_sequence_begin(const mutable_buffer &b) noexcept
    {
        return std::addressof(b);
    }
    constexpr const auto *buffer_sequence_begin(const const_buffer &b) noexcept
    {
        return std::addressof(b);
    }
    // Returns: std::addressof(b) + 1.
    constexpr const auto *buffer_sequence_end(const mutable_buffer &b) noexcept
    {
        return std::addressof(b) + 1;
    }
    constexpr const auto *buffer_sequence_end(const const_buffer &b) noexcept
    {
        return std::addressof(b) + 1;
    }

    // Returns: c.begin().
    template <class C>
    constexpr auto buffer_sequence_begin(C &c) noexcept -> decltype(c.begin())
        requires(requires { c.begin(); })
    {
        return c.begin();
    }
    template <class C>
    constexpr auto buffer_sequence_begin(const C &c) noexcept -> decltype(c.begin())
        requires(requires { c.begin(); })
    {
        return c.begin();
    }
    // Returns: c.end().
    template <class C>
    constexpr auto buffer_sequence_end(C &c) noexcept -> decltype(c.end())
        requires(requires { c.end(); })
    {
        return c.end();
    }
    template <class C>
    constexpr auto buffer_sequence_end(const C &c) noexcept -> decltype(c.end())
        requires(requires { c.end(); })
    {
        return c.end();
    }

}; // namespace mcs::net::io::buffer
