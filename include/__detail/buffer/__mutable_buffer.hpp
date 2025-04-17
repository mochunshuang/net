#pragma once

#include <algorithm>
#include <cstddef>

namespace mcs::net::buffer
{
    struct mutable_buffer
    {
      public:
        // constructors:
        // Postconditions: data_ == nullptr and size_ == 0
        constexpr mutable_buffer() noexcept = default;
        // Postconditions: data_ == p and size_ == n
        constexpr mutable_buffer(void *p, std::size_t n) noexcept : data_{p}, size_{n} {}
        // members:
        [[nodiscard]] constexpr void *data() const noexcept
        {
            return data_;
        }
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }
        /**
         * Effects: Sets data_ to static_cast<char*>(data_) + min(n, size_), and
         * then size_ to size_  - min(n, size_).
         */
        constexpr mutable_buffer &operator+=(std::size_t n) noexcept
        {
            const auto &offset = std::min(n, size_);
            data_ = static_cast<std::byte *>(data_) + offset;
            size_ -= offset;
            return *this;
        }

      private:
        void *data_{nullptr}; // exposition only
        std::size_t size_{0}; // exposition only
    };

}; // namespace mcs::net::buffer