#pragma once

#include "./__dynamic_vector_buffer.hpp"
#include "./__dynamic_string_buffer.hpp"

namespace mcs::net::io::buffer
{
    //  Returns: dynamic_vector_buffer<T, Allocator>(vec)
    template <class T, class Allocator>
    constexpr dynamic_vector_buffer<T, Allocator> dynamic_buffer(
        std::vector<T, Allocator> &vec) noexcept
    {
        return dynamic_vector_buffer<T, Allocator>{vec};
    }

    // Returns: dynamic_vector_buffer<T, Allocator>(vec, n)
    template <class T, class Allocator>
    constexpr dynamic_vector_buffer<T, Allocator> dynamic_buffer(
        std::vector<T, Allocator> &vec, std::size_t n) noexcept
    {
        return dynamic_vector_buffer<T, Allocator>{vec, n};
    }

    // Returns: dynamic_string_buffer<CharT, Traits, Allocator>(str)
    template <class CharT, class Traits, class Allocator>
    constexpr dynamic_string_buffer<CharT, Traits, Allocator> dynamic_buffer(
        std::basic_string<CharT, Traits, Allocator> &str) noexcept
    {
        return {str};
    }

    template <class CharT, class Traits, class Allocator>
    constexpr dynamic_string_buffer<CharT, Traits, Allocator> dynamic_buffer(
        std::basic_string<CharT, Traits, Allocator> &str, std::size_t n) noexcept
    {
        return {str, n};
    }

}; // namespace mcs::net::io::buffer
