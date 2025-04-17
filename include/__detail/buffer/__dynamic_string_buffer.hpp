#pragma once

#include "./__mutable_buffer.hpp"
#include "./__const_buffer.hpp"
#include "./__buffer.hpp"
#include <cassert>
#include <stdexcept>
#include <string>

namespace mcs::net::buffer
{
    // The dynamic_string_buffer class template requires that sizeof(CharT) == 1.
    template <class CharT, class Traits, class Allocator>
        requires(sizeof(CharT) == 1)
    struct dynamic_string_buffer
    {
      public:
        // types:
        using const_buffers_type = const_buffer;
        using mutable_buffers_type = mutable_buffer;

        // constructors:
        //  Initializes str_ with str, size_ with str.size(), and max_size_ with
        //  str.max_size()
        constexpr explicit dynamic_string_buffer(
            std::basic_string<CharT, Traits, Allocator> &str) noexcept
            : str_{str}, size_{str.size()}, k_max_size{str.max_size()}
        {
        }
        // Requires: str.size() <= maximum_size.
        // Effects: Initializes str_ with str, size_ with str.size(), and max_size_ with
        // maximum_size
        constexpr dynamic_string_buffer(std::basic_string<CharT, Traits, Allocator> &str,
                                        std::size_t maximum_size) noexcept
            : str_{str}, size_{str.size()}, k_max_size{maximum_size}
        {
        }
        dynamic_string_buffer(dynamic_string_buffer &&) = default;
        dynamic_string_buffer(const dynamic_string_buffer &) = default;
        dynamic_string_buffer &operator=(const dynamic_string_buffer &) = default;
        dynamic_string_buffer &operator=(dynamic_string_buffer &&) = default;
        ~dynamic_string_buffer() = default;

        // members:
        [[nodiscard]] constexpr std::size_t size() const noexcept
        {
            return size_;
        }
        [[nodiscard]] constexpr std::size_t max_size() const noexcept // NOLINT
        {
            return k_max_size;
        }
        [[nodiscard]] constexpr std::size_t capacity() const noexcept
        {
            return str_.capacity();
        }
        // Returns: buffer(str_, size_)
        [[nodiscard]] constexpr const_buffers_type data() const noexcept
        {
            return buffer(str_, size_);
        }
        // Effects: Performs str_.resize(size_ + n).
        // Returns : buffer(buffer(str_) + size_, n).
        // Remarks : length_error if size () + n exceeds max_size()
        constexpr mutable_buffers_type prepare(size_t n)
        {
            if (size_ + n > k_max_size)
                throw std::length_error{"prepare length_error in dynamic_string_buffer"};
            str_.resize(size_ + n);
            return buffer(buffer(str_) + size_, n);
        }
        // Effects: Performs:
        //  size_ += min(n, str_.size()- size_)
        //  str_.resize(size_);
        constexpr void commit(size_t n) noexcept
        {
            size_ += std::min(n, str_.size() - size_);
            str_.resize(size_);
        }

        /*
          Effects: Performs:
         std::size_t m = min(n, size_);
          str_.erase(0, m);
          size_-= m;
        */
        constexpr void consume(size_t n)
        {
            const auto &m = std::min(n, size_);
            str_.erase(0, m);
            size_ -= m;
        }

      private:
        std::basic_string<CharT, Traits, Allocator> &str_; // exposition only // NOLINT
        std::size_t size_;                                 // exposition only
        std::size_t k_max_size;                            // exposition only // NOLINT
    };

}; // namespace mcs::net::buffer