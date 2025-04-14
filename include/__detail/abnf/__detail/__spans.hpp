#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

#include "./__span.hpp"

#include "../__core_concepts.hpp"

namespace mcs::abnf::__detail
{
    template <std::size_t N>
    struct spans
    {
        using value_type = span;
        using reference = value_type &;

      private:
        std::array<value_type, N> spans_;

      public:
        template <typename... Args>
            requires(sizeof...(Args) == N) &&
                    (std::is_constructible_v<value_type, Args> && ...)
        constexpr explicit spans(Args &&...args) noexcept
            : spans_{std::forward<Args>(args)...}
        {
        }

        constexpr reference operator[](const std::size_t &index) noexcept
        {
            return spans_[index];
        }

        [[nodiscard]] consteval std::size_t size()
        {
            return N;
        }

        template <std::size_t idx, decays_to<spans> Self>
            requires(idx < N)
        constexpr decltype(auto) get(this Self &&self) noexcept
        {
            return std::forward_like<Self>(std::forward<Self>(self).spans_[idx]);
        }
    };

    namespace __detail
    {
        template <typename T>
        constexpr bool is_spans_type_v = false; // NOLINT

        template <std::size_t N>
        constexpr bool is_spans_type_v<::mcs::abnf::__detail::spans<N>> = true; // NOLINT
    }; // namespace __detail

    template <typename T>
    concept is_spans_type = __detail::is_spans_type_v<T>;

}; // namespace mcs::abnf::__detail

namespace std
{
    // for structured_binding
    template <std::size_t N>
    struct tuple_size<::mcs::abnf::__detail::spans<N>> // NOLINT
        : ::std::integral_constant<std::size_t, N>
    {
    };

    template <std::size_t N, ::std::size_t I>
    struct tuple_element<I, ::mcs::abnf::__detail::spans<N>> // NOLINT
    {
        using type =
            ::std::decay_t<decltype(::std::declval<::mcs::abnf::__detail::spans<N>>()
                                        .template get<I>())>;
    };

}; // namespace std

namespace std
{
    // for std::get<>
    template <std::size_t I, typename T>
        requires ::mcs::abnf::__detail::is_spans_type<decay_t<T>>
    constexpr auto get(T &&t) noexcept -> decltype(auto) // NOLINT
    {
        return std::forward<T>(t).template get<I>();
    }

}; // namespace std
