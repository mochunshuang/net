#pragma once

#include "./__mutable_buffer.hpp"
#include "./__const_buffer.hpp"

namespace mcs::net::io::buffer
{
    /**
     * @brief Returns: A mutable_buffer equivalent to
     * mutable_buffer(
     * static_cast<char*>(b.data()) + min(n, b.size()),
     * b.size()- min(n, b.size()));
     */
    constexpr mutable_buffer operator+(const mutable_buffer &b, std::size_t n) noexcept
    {
        const auto k_offset = n < b.size() ? n : b.size();
        return {static_cast<std::byte *>(b.data()) + k_offset, b.size() - k_offset};
    }
    constexpr mutable_buffer operator+(size_t n, const mutable_buffer &b) noexcept
    {
        return b + n;
    }

    /**
     * @brief Returns: A const_buffer equivalent to
     * const_buffer(
     * static_cast<const char*>(b.data()) + min(n, b.size()),
     * b.size()- min(n, b.size()));r
     */
    constexpr const_buffer operator+(const const_buffer &b, std::size_t n) noexcept
    {
        const auto k_offset = n < b.size() ? n : b.size();
        return {static_cast<const std::byte *>(b.data()) + k_offset, b.size() - k_offset};
    }
    constexpr const_buffer operator+(size_t n, const const_buffer &b) noexcept
    {
        return b + n;
    }

}; // namespace mcs::net::io::buffer
