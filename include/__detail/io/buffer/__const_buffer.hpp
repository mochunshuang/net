#pragma once

#include "./__mutable_buffer.hpp"

namespace mcs::net::io::buffer
{
    struct const_buffer
    {
        // [constructors]
        // Construct an empty buffer.
        const_buffer() noexcept : data_{nullptr}, size_{0} {}
        // Construct a buffer to represent a given memory range.
        const_buffer(const void *data, std::size_t size) noexcept
            : data_{data}, size_{size}
        {
        }
        // Construct a non-modifiable buffer from a modifiable one.
        // NOTE: 允许隐式转换
        const_buffer(const mutable_buffer &buf) noexcept // NOLINT
            : data_(buf.data()), size_(buf.size())
        {
        }
        template <class T, size_t Extent>
        explicit const_buffer(const std::span<T, Extent> &span) noexcept
            : data_{span.data()}, size_{span.size()}
        {
        }
        // [members:]
        // Get a pointer to the beginning of the memory range.
        [[nodiscard]] const void *data() const noexcept
        {
            return data_;
        }
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }

        // NOTE:  n > size_ may cause bug
        //  Move the start of the buffer by the specified number of bytes.
        constexpr const_buffer &operator+=(std::size_t n) noexcept
        {
            const auto k_offset = n < size_ ? n : size_;
            data_ = static_cast<const std::byte *>(data_) + k_offset;
            size_ -= k_offset;
            return *this;
        }

      private:
        const void *data_;
        std::size_t size_;
    };

}; // namespace mcs::net::io::buffer