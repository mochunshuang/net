#pragma once

#include "./__mutable_buffer.hpp"
#include "./__const_buffer.hpp"
#include "./__buffer.hpp"
#include <cassert>
#include <stdexcept>
#include <vector>

namespace mcs::net::buffer
{

    template <class T, class Allocator>
    struct dynamic_vector_buffer
    {
      public:
        // types:
        using const_buffers_type = const_buffer;
        using mutable_buffers_type = mutable_buffer;

        // constructors:
        // Effects: Initializes vec_ with vec, size_ with vec.size(), and max_size_ with
        // vec.max_size()
        constexpr explicit dynamic_vector_buffer(std::vector<T, Allocator> &vec) noexcept
            : vec_{vec}, size_{vec.size()}, k_max_size{vec.max_size()}
        {
        }
        // Requires: vec.size() <= maximum_size
        //  Initializes vec_ with vec, size_ with vec.size(), and max_size_ with
        //  maximum_size.
        constexpr dynamic_vector_buffer(std::vector<T, Allocator> &vec,
                                        std::size_t maximum_size) noexcept
            : vec_{vec}, size_{vec.size()}, k_max_size{maximum_size}
        {
            assert(vec.size() <= maximum_size);
        }
        dynamic_vector_buffer(dynamic_vector_buffer &&) = default;

        // othre constructors
        dynamic_vector_buffer(const dynamic_vector_buffer &) = default;
        dynamic_vector_buffer &operator=(const dynamic_vector_buffer &) = default;
        dynamic_vector_buffer &operator=(dynamic_vector_buffer &&) = default;
        ~dynamic_vector_buffer() = default;

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
            return vec_.capacity();
        }
        [[nodiscard]] constexpr const_buffers_type data() const noexcept
        {
            return buffer(vec_, size_);
        }
        /**
         * Effects: Performs vec_.resize(size_ + n).
         * Returns: buffer(buffer(vec_) + size_, n).
         * Remarks: length_error if size() + n exceeds max_size()
         */
        constexpr mutable_buffers_type prepare(std::size_t n)
        {
            if (size_ + n > k_max_size)
                throw std::length_error{"prepare length_error in dynamic_vector_buffer"};

            vec_.resize(size_ + n);
            return buffer(buffer(vec_) + size_, n);
        }
        /**
         * Effects: Performs:
         * size_ += min(n, vec_.size()- size_);
         * vec_.resize(size_);
         */
        constexpr void commit(std::size_t n)
        {
            size_ += std::min(n, vec_.size() - size_);
            vec_.resize(size_);
        }
        /**
         * Effects: Performs:
         *std::size_t m = min(n, size_);
         * vec_.erase(vec_.begin(), vec_.begin() + m);;
         * size_-= m;
         */
        constexpr void consume(std::size_t n)
        {
            const auto &m = std::min(n, size_);
            vec_.erase(vec_.begin(), vec_.begin() + m);
            size_ -= m;
        }

      private:
        std::vector<T, Allocator> &vec_; // exposition only // NOLINT
        std::size_t size_;               // exposition only
        const std::size_t k_max_size;    // exposition only // NOLINT
    };

}; // namespace mcs::net::buffer