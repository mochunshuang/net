#pragma once

#include "./__buffer.hpp"
#include <stdexcept>
#include <limits>

namespace mcs::net::io::buffer
{
    template <class T, class Allocator>
    struct dynamic_vector_buffer
    {
        // The type used to represent a sequence of constant buffers that refers to the
        // underlying memory.
        using const_buffers_type = const_buffer;

        // The type used to represent a sequence of mutable buffers that refers to the
        // underlying memory.
        using mutable_buffers_type = mutable_buffer;

        explicit dynamic_vector_buffer(
            std::vector<T, Allocator> &v,
            std::size_t maximum_size = std::numeric_limits<std::size_t>::max()) noexcept
            : vector_{v}, k_max_size{maximum_size}
        {
        }
        dynamic_vector_buffer(const dynamic_vector_buffer &other) = default;
        dynamic_vector_buffer(dynamic_vector_buffer &&) = default;
        dynamic_vector_buffer &operator=(const dynamic_vector_buffer &) = default;
        dynamic_vector_buffer &operator=(dynamic_vector_buffer &&) = default;
        ~dynamic_vector_buffer() = default;

        [[nodiscard]] constexpr std::size_t max_size() const noexcept // NOLINT
        {
            return k_max_size;
        }
        // Get the current size of the underlying memory.
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return vector_.size() < max_size() ? vector_.size() : max_size();
        }
        // Get the maximum size that the buffer may grow to without triggering
        [[nodiscard]] constexpr std::size_t capacity() const noexcept
        {
            return vector_.capacity() < max_size() ? vector_.capacity() : max_size();
        }
        // Get a sequence of buffers that represents the underlying memory.
        constexpr mutable_buffers_type data(std::size_t pos, std::size_t n) noexcept
        {
            return buffer(buffer(vector_, k_max_size) + pos, n);
        }
        [[nodiscard]] constexpr const_buffers_type data(std::size_t pos,
                                                        std::size_t n) const noexcept
        {
            return buffer(buffer(vector_, k_max_size) + pos, n);
        }

        // Grow the underlying memory by the specified number of bytes
        constexpr void grow(std::size_t n)
        {
            if (size() > max_size() || max_size() - size() < n)
                throw std::length_error("dynamic_vector_buffer too long");
            vector_.resize(size() + n);
        }
        // Shrink the underlying memory by the specified number of bytes
        constexpr void shrink(std::size_t n)
        {
            vector_.resize(n > size() ? 0 : size() - n);
        }
        // NOTE: 可能异常
        constexpr void consume(std::size_t n)
        {
            vector_.erase(vector_.begin(), vector_.begin() + std::min(size(), n));
        }

      private:
        std::vector<T, Allocator> &vector_; // NOLINT
        const std::size_t k_max_size{};     // NOLINT
    };
}; // namespace mcs::net::io::buffer