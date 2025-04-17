#pragma once

#include "./__mutable_buffer.hpp"
#include "./__const_buffer.hpp"

#include <array>
#include <vector>
#include <string>

namespace mcs::net::buffer
{

    // NOTE: 约束T，以及对 vector<>、basic_string<> 情况的特殊说明
    /**
     * 1、In the functions below, T shall be a trivially copyable or standard-layout type
     * 2、For the function overloads below that accept an argument of type vector<>, the
     * buffer objects returned are invalidated by any vector operation that also
     * invalidates all references, pointers and iterators referring to the elements in the
     * sequence
     * 3、For the function overloads below that accept an argument of type basic_string<>,
     * the buffer objects returned are invalidated according to the rules defined for
     * invalidation of references, pointers and iterators referring to elements of the
     * sequence
     */

    // Returns: mutable_buffer(p, n).
    constexpr mutable_buffer buffer(void *p, std::size_t n) noexcept
    {
        return {p, n};
    }
    // Returns: const_buffer(p, n)
    constexpr const_buffer buffer(const void *p, std::size_t n) noexcept
    {
        return {p, n};
    }

    // Returns: b.
    constexpr mutable_buffer buffer(const mutable_buffer &b) noexcept
    {
        return b;
    }
    // Returns: mutable_buffer(b.data(), min(b.size(), n))
    constexpr mutable_buffer buffer(const mutable_buffer &b, std::size_t n) noexcept
    {
        return {b.data(), std::min(b.size(), n)};
    }

    // Returns: b.
    constexpr const_buffer buffer(const const_buffer &b) noexcept
    {
        return b;
    }
    // Returns: const_buffer(b.data(), min(b.size(), n)).
    constexpr const_buffer buffer(const const_buffer &b, std::size_t n) noexcept
    {
        return {b.data(), std::min(b.size(), n)};
    }

    /**
     * Returns: buffer(
     * begin(data) != end(data) ? std::addressof(*begin(data)) : nullptr,
     * (end(data)- begin(data)) * sizeof(*begin(data)));
     */
    template <class T, std::size_t N>
    constexpr mutable_buffer buffer(T (&data)[N]) noexcept // NOLINT
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, std::size_t N>
    constexpr const_buffer buffer(const T (&data)[N]) noexcept // NOLINT
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, std::size_t N>
    constexpr mutable_buffer buffer(std::array<T, N> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    };
    template <class T, std::size_t N>
    constexpr const_buffer buffer(std::array<const T, N> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, std::size_t N>
    constexpr const_buffer buffer(const std::array<T, N> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, class Allocator>
    constexpr mutable_buffer buffer(std::vector<T, Allocator> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, class Allocator>
    constexpr const_buffer buffer(const std::vector<T, Allocator> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class CharT, class Traits, class Allocator>
    constexpr mutable_buffer buffer(
        std::basic_string<CharT, Traits, Allocator> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class CharT, class Traits, class Allocator>
    constexpr const_buffer buffer(
        const std::basic_string<CharT, Traits, Allocator> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class CharT, class Traits>
    constexpr const_buffer buffer(std::basic_string_view<CharT, Traits> data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }

    /**
     * Returns: buffer(buffer(data), n)
     */
    template <class T, std::size_t N>
    constexpr mutable_buffer buffer(T (&data)[N], std::size_t n) noexcept // NOLINT
    {
        return buffer(buffer(data), n);
    }
    template <class T, std::size_t N>
    constexpr const_buffer buffer(const T (&data)[N], std::size_t n) noexcept // NOLINT
    {
        return buffer(buffer(data), n);
    }
    template <class T, std::size_t N>
    constexpr mutable_buffer buffer(std::array<T, N> &data, std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class T, std::size_t N>
    constexpr const_buffer buffer(std::array<const T, N> &data, std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class T, std::size_t N>
    constexpr const_buffer buffer(const std::array<T, N> &data, std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class T, class Allocator>
    constexpr mutable_buffer buffer(std::vector<T, Allocator> &data,
                                    std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class T, class Allocator>
    constexpr const_buffer buffer(const std::vector<T, Allocator> &data,
                                  std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class CharT, class Traits, class Allocator>
    constexpr mutable_buffer buffer(std::basic_string<CharT, Traits, Allocator> &data,
                                    std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class CharT, class Traits, class Allocator>
    constexpr const_buffer buffer(const std::basic_string<CharT, Traits, Allocator> &data,
                                  std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }
    template <class CharT, class Traits>
    constexpr const_buffer buffer(std::basic_string_view<CharT, Traits> data,
                                  std::size_t n) noexcept
    {
        return buffer(buffer(data), n);
    }

}; // namespace mcs::net::buffer