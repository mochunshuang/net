#pragma once

#include "./__spans.hpp"

namespace mcs::abnf::__detail
{
    template <std::size_t N>
    struct result
    {
      private:
        spans<N> spans_;

      public:
        template <typename... Args>
            requires(sizeof...(Args) == N) && (std::is_constructible_v<span, Args> && ...)
        constexpr explicit result(Args &&...e) noexcept : spans_{std::forward<Args>(e)...}
        {
        }

        [[nodiscard]] consteval static std::size_t size()
        {
            return N;
        }

        template <std::size_t I, decays_to<result> Self>
            requires(I < N)
        constexpr decltype(auto) get(this Self &&self) noexcept
        {
            return std::forward_like<Self>(
                std::forward<Self>(self).spans_.template get<I>());
        }
    };
}; // namespace mcs::abnf::__detail

namespace std
{
    // for structured_binding
    template <std::size_t N>
    struct tuple_size<::mcs::abnf::__detail::result<N>> // NOLINT
        : ::std::integral_constant<::std::size_t, N>
    {
    };

    template <std::size_t N, ::std::size_t I>
    struct tuple_element<I, ::mcs::abnf::__detail::result<N>> // NOLINT
    {
        using type =
            ::std::decay_t<decltype(::std::declval<::mcs::abnf::__detail::result<N>>()
                                        .template get<I>())>;
    };

}; // namespace std