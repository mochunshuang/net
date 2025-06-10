#pragma once

#include "./__buffer.hpp"
#include <stdexcept>
#include <limits>

namespace mcs::net::io::buffer
{
    // The dynamic_string_buffer class template requires that sizeof(CharT) == 1.
    template <class CharT, class Traits, class Allocator>
        requires(sizeof(CharT) == 1)
    struct dynamic_string_buffer
    {
        // The type used to represent a sequence of constant buffers that refers to the
        // underlying memory.
        using const_buffers_type = const_buffer;

        // The type used to represent a sequence of mutable buffers that refers to the
        // underlying memory.
        using mutable_buffers_type = mutable_buffer;

        // Construct a dynamic buffer from a string.
        // Requires: str.size() <= maximum_size.
        constexpr explicit dynamic_string_buffer(
            std::basic_string<CharT, Traits, Allocator> &str,
            std::size_t maximum_size = std::numeric_limits<std::size_t>::max()) noexcept
            : string_{str}, k_max_size{maximum_size}
        {
        }
        // Copy and Move construct a dynamic buffer.
        dynamic_string_buffer(const dynamic_string_buffer &other) = default;
        dynamic_string_buffer(dynamic_string_buffer &&other) = default;
        dynamic_string_buffer &operator=(const dynamic_string_buffer &) = default;
        dynamic_string_buffer &operator=(dynamic_string_buffer &&) = default;
        ~dynamic_string_buffer() = default;

        // The allowed maximum size of the underlying memory.
        [[nodiscard]] constexpr std::size_t max_size() const noexcept // NOLINT
        {
            return k_max_size;
        }
        // Get the current size of the underlying memory.
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return string_.size() < max_size() ? string_.size() : max_size();
        }
        // Get the maximum size that the buffer may grow to without triggering
        [[nodiscard]] constexpr std::size_t capacity() const noexcept
        {
            return string_.capacity() < max_size() ? string_.capacity() : max_size();
        }
        // Get a sequence of buffers that represents the underlying memory.
        constexpr mutable_buffers_type data(std::size_t pos, std::size_t n) noexcept
        {
            return buffer(buffer(string_, k_max_size) + pos, n);
        }
        [[nodiscard]] constexpr const_buffers_type data(std::size_t pos,
                                                        std::size_t n) const noexcept
        {
            return buffer(buffer(string_, k_max_size) + pos, n);
        }

        // Grow the underlying memory by the specified number of bytes
        constexpr void grow(std::size_t n)
        {
            if (size() > max_size() || max_size() - size() < n)
                throw std::length_error("dynamic_string_buffer too long");
            string_.resize(size() + n);
        }
        // Shrink the underlying memory by the specified number of bytes
        constexpr void shrink(std::size_t n)
        {
            string_.resize(n > size() ? 0 : size() - n);
        }
        // NOTE: 可能异常
        constexpr void consume(std::size_t n)
        {
            string_.erase(0, std::min(size(), n));
        }

      private:                                                // NOTE: 代理 string
        std::basic_string<CharT, Traits, Allocator> &string_; // NOLINT
        const std::size_t k_max_size;                         // NOLINT
    };
}; // namespace mcs::net::io::buffer