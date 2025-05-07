#pragma once

#include "../detail/__types.hpp"
#include "../tool/__to_upper.hpp"
#include "../tool/__to_lower.hpp"
#include <algorithm>
#include <string>
#include <array>

namespace mcs::abnf::generate
{
    template <size_t N>
    struct FixedString
    {
        static constexpr size_t size = N;                    // NOLINT
        char value[N]{};                                     // NOLINT
        constexpr FixedString(const char (&str)[N]) noexcept // NOLINT
        {
            std::copy_n(str, N, value);
        }
    };

    template <FixedString Str, bool Insensitive = true>
    struct String
    {
        constexpr static size_t k_count = decltype(Str)::size - 1;

        constexpr static auto k_string = [] consteval {
            std::array<detail::octet, k_count> arr{};
            for (size_t i = 0; i < k_count; ++i)
            {
                arr[i] = static_cast<detail::octet>(Str.value[i]);
            }
            return arr;
        }();

        struct __type
        {
            using domain = String;
            detail::octets_view value;
        };
        using rule_concept = detail::rule_t;
        using result_type = __type;

        constexpr static auto operator()(detail::parser_ctx_ref ctx) noexcept
            -> detail::consumed_result
        {
            if (ctx.remain() < k_count)
                return std::nullopt;

            if constexpr (Insensitive)
            {
                for (size_t i = 0; i < k_count; ++i)
                {
                    const auto k_current = ctx.root_span[ctx.cur_index + i];
                    if (tool::to_upper(k_current) != k_string[i] &&
                        tool::to_lower(k_current) != k_string[i])
                    {
                        return std::nullopt;
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < k_count; ++i)
                {
                    if (ctx.root_span[ctx.cur_index + i] != k_string[i])
                    {
                        return std::nullopt;
                    }
                }
            }
            ctx.cur_index += k_count;
            return detail::make_consumed_result(k_count);
        }
        static constexpr auto parse(detail::parser_ctx_ref ctx) noexcept
            -> std::optional<result_type>
        {
            auto begin{ctx.cur_index};
            auto ret = operator()(ctx);
            return ret ? std::make_optional(
                             result_type{.value = ctx.root_span.subspan(begin, *ret)})
                       : std::nullopt;
        }
        static constexpr auto buildString(const result_type &ctx) noexcept
        {
            return std::string(ctx.value.begin(), ctx.value.end());
        }
    };
    template <FixedString Str>
    using StringSensitive = String<Str, false>;
}; // namespace mcs::abnf::generate