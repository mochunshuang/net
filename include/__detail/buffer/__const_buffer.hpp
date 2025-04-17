#pragma once

#include "./__mutable_buffer.hpp"

namespace mcs::net::buffer
{

    struct const_buffer
    {
      public:
        // constructors:
        // Postconditions: data_ == nullptr and size_ == 0
        constexpr const_buffer() noexcept = default;
        // Postconditions: data_ == p and size_ == n
        constexpr const_buffer(const void *p, std::size_t n) noexcept : data_{p}, size_{n}
        {
        }
        // Postconditions: data_ == b.data() and size_ == b.size()
        constexpr explicit const_buffer(const mutable_buffer &b) noexcept
            : data_{b.data()}, size_{b.size()} {};

        // members:
        [[nodiscard]] constexpr const void *data() const noexcept
        {
            return data_;
        }
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }
        /**
         * @brief  Sets data_ to static_cast<const char*>(data_) + min(n, size_), and then
         * size_ to size_- min(n, size_).
         */
        constexpr const_buffer &operator+=(std::size_t n) noexcept
        {
            const auto &offset = std::min(n, size_);
            data_ = static_cast<const std::byte *>(data_) + offset;
            size_ -= offset;
            return *this;
        }

      private:
        const void *data_{nullptr}; // exposition only
        std::size_t size_{0};       // exposition only
    };

}; // namespace mcs::net::buffer