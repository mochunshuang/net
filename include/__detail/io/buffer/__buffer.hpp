#pragma once

#include "./__mutable_buffer.hpp"
#include "./__const_buffer.hpp"

#include <array>
#include <type_traits>
#include <vector>
#include <string>

namespace mcs::net::io::buffer
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
    constexpr mutable_buffer buffer(void *p, std::size_t max_size_in_bytes) noexcept
    {
        return {p, max_size_in_bytes};
    }
    // Returns: const_buffer(p, n)
    constexpr const_buffer buffer(const void *p, std::size_t max_size_in_bytes) noexcept
    {
        return {p, max_size_in_bytes};
    }

    // Returns: b.
    constexpr mutable_buffer buffer(const mutable_buffer &b) noexcept
    {
        return b;
    }
    // Returns: mutable_buffer(b.data(), min(b.size(), n))
    constexpr mutable_buffer buffer(const mutable_buffer &b,
                                    std::size_t max_size_in_bytes) noexcept
    {
        return {b.data(), b.size() < max_size_in_bytes ? b.size() : max_size_in_bytes};
    }

    // Returns: b.
    constexpr const_buffer buffer(const const_buffer &b) noexcept
    {
        return b;
    }
    // Returns: const_buffer(b.data(), min(b.size(), n)).
    constexpr const_buffer buffer(const const_buffer &b,
                                  std::size_t max_size_in_bytes) noexcept
    {
        return {b.data(), b.size() < max_size_in_bytes ? b.size() : max_size_in_bytes};
    }

    /**
     * Returns: buffer(
     * begin(data) != end(data) ? std::addressof(*begin(data)) : nullptr,
     * (end(data)- begin(data)) * sizeof(*begin(data)));
     */
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr mutable_buffer buffer(T (&data)[N]) noexcept // NOLINT
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(const T (&data)[N]) noexcept // NOLINT
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr mutable_buffer buffer(std::array<T, N> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    };
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(std::array<const T, N> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(const std::array<T, N> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, class Allocator>
        requires(std::is_standard_layout_v<T>)
    constexpr mutable_buffer buffer(std::vector<T, Allocator> &data) noexcept
    {
        return buffer(std::begin(data) != std::end(data)
                          ? std::addressof(*std::begin(data))
                          : nullptr,
                      (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
    }
    template <class T, class Allocator>
        requires(std::is_standard_layout_v<T>)
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
        requires(std::is_standard_layout_v<T>) // NOLINTNEXTLINE
    constexpr mutable_buffer buffer(T (&data)[N], std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), N * sizeof(T) < max_size_in_bytes
                                        ? N * sizeof(T)
                                        : max_size_in_bytes);
    }
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(const T (&data)[N], // NOLINT
                                  std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), N * sizeof(T) < max_size_in_bytes
                                        ? N * sizeof(T)
                                        : max_size_in_bytes);
    }
    // array
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr mutable_buffer buffer(std::array<T, N> &data,
                                    std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(T) < max_size_in_bytes
                                        ? data.size() * sizeof(T)
                                        : max_size_in_bytes);
    }
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(std::array<const T, N> &data,
                                  std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(T) < max_size_in_bytes
                                        ? data.size() * sizeof(T)
                                        : max_size_in_bytes);
    }
    template <class T, std::size_t N>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(const std::array<T, N> &data,
                                  std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(T) < max_size_in_bytes
                                        ? data.size() * sizeof(T)
                                        : max_size_in_bytes);
    }

    // vector
    template <class T, class Allocator>
        requires(std::is_standard_layout_v<T>)
    constexpr mutable_buffer buffer(std::vector<T, Allocator> &data,
                                    std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(T) < max_size_in_bytes
                                        ? data.size() * sizeof(T)
                                        : max_size_in_bytes);
    }
    template <class T, class Allocator>
        requires(std::is_standard_layout_v<T>)
    constexpr const_buffer buffer(const std::vector<T, Allocator> &data,
                                  std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(T) < max_size_in_bytes
                                        ? data.size() * sizeof(T)
                                        : max_size_in_bytes);
    }

    // basic_string
    template <class CharT, class Traits, class Allocator>
    constexpr mutable_buffer buffer(std::basic_string<CharT, Traits, Allocator> &data,
                                    std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(CharT) < max_size_in_bytes
                                        ? data.size() * sizeof(CharT)
                                        : max_size_in_bytes);
    }
    template <class CharT, class Traits, class Allocator>
    constexpr const_buffer buffer(const std::basic_string<CharT, Traits, Allocator> &data,
                                  std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(CharT) < max_size_in_bytes
                                        ? data.size() * sizeof(CharT)
                                        : max_size_in_bytes);
    }
    // basic_string_view
    template <class CharT, class Traits>
    constexpr const_buffer buffer(std::basic_string_view<CharT, Traits> data,
                                  std::size_t max_size_in_bytes) noexcept
    {
        return buffer(buffer(data), data.size() * sizeof(CharT) < max_size_in_bytes
                                        ? data.size() * sizeof(CharT)
                                        : max_size_in_bytes);
    }

    // Create a new modifiable buffer from a span.
    template <typename T, std::size_t Extent>
        requires(sizeof(T) == 1)
    inline mutable_buffer buffer(const std::span<T, Extent> &span)
    {
        return mutable_buffer(span);
    }
    template <typename T, std::size_t Extent>
        requires(sizeof(T) == 1)
    inline mutable_buffer buffer(const std::span<T, Extent> &span,
                                 std::size_t max_size_in_bytes)
    {
        return buffer(mutable_buffer(span), max_size_in_bytes);
    }
    // Create a new non-modifiable buffer from a span.
    template <typename T, std::size_t Extent>
        requires(sizeof(T) == 1)
    inline const_buffer buffer(const std::span<const T, Extent> &span)
    {
        return const_buffer(span);
    }
    template <typename T, std::size_t Extent>
        requires(sizeof(T) == 1)
    inline const_buffer buffer(const std::span<const T, Extent> &span,
                               std::size_t max_size_in_bytes)
    {
        return buffer(const_buffer(span), max_size_in_bytes);
    }

}; // namespace mcs::net::io::buffer
