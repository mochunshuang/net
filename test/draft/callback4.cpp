#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#include <iostream>
#include "./__product_type.hpp"

// NOLINTBEGIN

using octet = std::uint8_t;
using octets_view = std::span<const octet>;
using octets_view_in = const octets_view &;
using octet_in = const octet &;

using consumed_result = std::optional<std::size_t>;
struct parser_ctx
{
    octets_view root_span;   // NOLINT
    std::size_t cur_index{}; // NOLINT
    std::size_t end_index{}; // NOLINT

    [[nodiscard]] constexpr auto size() const noexcept
    {
        return end_index - cur_index;
    }
    [[nodiscard]] constexpr auto empty() const noexcept
    {
        return end_index == cur_index;
    }
    [[nodiscard]] constexpr auto valid() const noexcept
    {
        return cur_index < end_index;
    }
};

constexpr parser_ctx make_parser_ctx(octets_view_in span) noexcept
{
    return {.root_span = span, .cur_index = 0, .end_index = span.size()};
}
using const_parser_ctx = const parser_ctx &;

template <octet C>
struct SensitiveChar
{
    constexpr auto operator()(const_parser_ctx ctx) const noexcept -> consumed_result
    {
        return !ctx.empty() && ctx.root_span[ctx.cur_index] == C
                   ? std::make_optional(size_t{1})
                   : std::nullopt;
    }
};

template <typename... T>
struct make_sequence : draft::__detail::product_type<T...>
{
    constexpr auto operator()(const_parser_ctx ctx) noexcept -> consumed_result
    {
        auto apply_all =
            [&]<typename... Rule>(Rule &&...rule) noexcept -> consumed_result {
            std::size_t total = 0;
            parser_ctx copy_ctx = ctx;
            auto apply_rule = [&]<typename R>(R &&r) noexcept -> bool {
                if (auto res = std::forward<R>(r)(copy_ctx))
                {
                    total += *res;
                    copy_ctx.cur_index += *res;
                    return true;
                }
                return false;
            };
            return static_cast<bool>((apply_rule(std::forward<Rule>(rule)) && ...))
                       ? std::make_optional(total)
                       : std::nullopt;
        };
        return this->apply(std::move(apply_all));
    }
};
template <typename... T> // NOLINTNEXTLINE
make_sequence(T &&...r) -> make_sequence<std::decay_t<T>...>;

template <typename Rule, typename Callback>
struct with_callback
{
    std::decay_t<Rule> r;
    Callback &callback;

    // NOLINTNEXTLINE
    constexpr with_callback(Rule &&r, Callback &c) noexcept
        : r{std::forward<Rule>(r)}, callback{c}
    {
    }
    constexpr auto operator()(const_parser_ctx sp) noexcept -> consumed_result
    {
        auto ret = r(sp);
        if (ret)
            callback(sp);
        return ret;
    }
};

constexpr auto test()
{
    constexpr std::array<octet, 3> arr = {'a', 'x', 'x'};
    using R = SensitiveChar<'x'>;
    int value = 0;
    int idx{0};
    std::array<size_t, 2> indexs;
    auto test_callback = [&](const_parser_ctx ctx) noexcept {
        value++;
        indexs[idx++] = ctx.cur_index;
    };

    auto rule = make_sequence{SensitiveChar<'a'>{}, with_callback(R{}, test_callback),
                              with_callback(R{}, test_callback)};
    auto ret = rule(make_parser_ctx(arr));

    assert(1 == 1);
    assert(indexs[0] == 1);
    assert(indexs[1] == 2);

    return std::make_pair(value, *ret);
}

int main()
{
    {
        constexpr auto p1 = test();
        static_assert(p1.first == 2);
        static_assert(p1.second == 3);
        {
            // structured binding declaration cannot be 'constexpr'
            auto [a, b] = test();
            assert(a == 2);
            assert(b == 3);
        }
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND