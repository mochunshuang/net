#pragma once

#include <cstddef>
#include <span>

namespace mcs::net::io::buffer
{
    struct mutable_buffer
    {
        // [constructors]
        // Construct an empty buffer.
        mutable_buffer() noexcept : data_{nullptr}, size_{0} {}
        // Construct a buffer to represent a given memory range.
        mutable_buffer(void *data, std::size_t size) noexcept : data_{data}, size_{size}
        {
        }

        template <class T, size_t Extent>
        explicit mutable_buffer(const std::span<T, Extent> &span) noexcept
            : data_{span.data()}, size_{span.size()}
        {
        }

        // [members:]
        // Get a pointer to the beginning of the memory range.
        [[nodiscard]] void *data() const noexcept
        {
            return data_;
        }
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }

        // NOTE:  n > size_ may cause bug
        //  Move the start of the buffer by the specified number of bytes.
        constexpr mutable_buffer &operator+=(std::size_t n) noexcept
        {
            const auto k_offset = n < size_ ? n : size_;
            data_ = static_cast<std::byte *>(data_) + k_offset;
            size_ -= k_offset;
            return *this;
        }

      private:
        void *data_;
        std::size_t size_;
    };

}; // namespace mcs::net::io::buffer