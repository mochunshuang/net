#include <array>
#include <cassert>
#include <cstdint>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#include <iostream>
#include "./__product_type.hpp"

// NOLINTBEGIN

using octet = uint8_t;
using span_param_in = const std::span<const octet> &;
using consumed_result = std::optional<size_t>;

template <octet C>
struct CharRule
{
    constexpr consumed_result operator()(span_param_in sp) const noexcept
    {
        return (!sp.empty() && sp[0] == C) ? std::make_optional(size_t{1}) : std::nullopt;
    }
};

template <typename Rule, typename Callback>
struct WithCallback
{
    Callback callback;
    constexpr consumed_result operator()(span_param_in sp) const noexcept
    {
        auto ret = Rule{}(sp);
        if (ret)
            callback();
        return ret;
    }
};
template <typename... Rules>
constexpr auto SEQUENCE(Rules... rules) noexcept // NOLINT
{
    return [=](span_param_in sp) constexpr noexcept -> consumed_result {
        std::size_t total = 0;
        auto remaining = sp;
        bool success = true;
        (
            [&](auto rule) constexpr noexcept {
                if (!success)
                    return;
                if (auto res = rule(remaining))
                {
                    total += *res;
                    remaining = remaining.subspan(*res);
                }
                else
                {
                    success = false;
                }
            }(rules),
            ...);

        return success ? std::make_optional(total) : std::nullopt;
    };
}

template <typename... T>
struct sequence
{
  private:
    draft::__detail::product_type<T...> impl;

  public:
    constexpr explicit sequence(T &&...t) : impl{std::forward<T>(t)...} {}

    constexpr auto operator()(span_param_in ctx) noexcept -> consumed_result
    {
        std::size_t total = 0;
        auto apply_rule = [&]<typename R>(R &&r) constexpr noexcept -> bool {
            if (auto res = std::forward<R>(r)(ctx))
            {
                total += *res;
                return true;
            }
            return false;
        };
        return static_cast<bool>(impl.apply(apply_rule)) ? std::make_optional(total)
                                                         : std::nullopt;
    }
};
template <typename... T> // NOLINTNEXTLINE
sequence(T &&...r) -> sequence<std::remove_cvref_t<T>...>;

template <typename... T>
struct make_sequence : draft::__detail::product_type<T...>
{

    constexpr auto operator()(span_param_in copy_ctx) noexcept -> consumed_result
    {
        auto apply_all =
            [&]<typename... Rule>(Rule &&...rule) constexpr noexcept -> consumed_result {
            std::size_t total = 0;

            auto apply_rule = [&]<typename R>(R &&r) constexpr noexcept -> bool {
                if (auto res = std::forward<R>(r)(copy_ctx))
                {
                    total += *res;
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
constexpr auto WithCallback2(Rule &&r, Callback &callback) noexcept
{
    return [r = std::forward<Rule>(r),
            &callback](span_param_in sp) constexpr noexcept -> consumed_result {
        static_assert(std::is_reference_v<decltype(callback)>);
        auto ret = r(sp);
        if (ret)
            callback();
        return ret;
    };
};

template <typename Rule, typename Callback>
struct with_callback
{
    std::decay_t<Rule> r;
    Callback &callback;

    // NOLINTNEXTLINE
    constexpr with_callback(Rule &&r, Callback &c) noexcept
        : r{std::forward<Rule>(r)}, callback(c)
    {
    }
    constexpr auto operator()(span_param_in sp) noexcept -> consumed_result
    {
        auto ret = r(sp);
        if (ret)
            callback();
        return ret;
    }
};

constexpr auto test()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    using WC_type = WithCallback<R, decltype(test_callback) &>;
    WC_type new_R{test_callback};

    auto ret = SEQUENCE(new_R)(arr);

    return std::make_pair(value, *ret);
}
constexpr auto test2()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    auto new_R = WithCallback2(R{}, test_callback);

    auto ret = SEQUENCE(new_R)(arr);

    return std::make_pair(value, *ret);
}

constexpr auto foo(auto &&...args) noexcept
{
    return [... args = std::forward<decltype(args)>(args)] {
        return (0 + ... + args);
    };
}

// NOTE: 越复杂越不容易优化
constexpr auto test3()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    auto new_R = WithCallback2(R{}, test_callback);

    auto rule = sequence{std::move(new_R)};
    auto ret = rule(arr);

    return std::make_pair(value, *ret);
}
constexpr auto test4()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    auto new_R = WithCallback2(R{}, test_callback);

    auto ret = sequence{std::move(new_R)}(arr);

    return std::make_pair(value, *ret);
}

constexpr auto test5()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    auto new_R = WithCallback2(R{}, test_callback);
    auto rule = SEQUENCE(new_R);
    auto ret = rule(arr);

    return std::make_pair(value, *ret);
}

constexpr auto test6()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    auto new_R = WithCallback2(R{}, test_callback);

    auto rule = make_sequence{std::move(new_R), WithCallback2(R{}, test_callback)};
    auto ret = rule(arr);

    return std::make_pair(value, *ret);
}
constexpr auto test7()
{
    constexpr std::array<octet, 2> arr = {'x', 'x'};
    using R = CharRule<'x'>;
    int value = 0;
    auto test_callback = [&value] {
        value = 1;
    };

    auto rule = make_sequence{with_callback(R{}, test_callback),
                              with_callback(R{}, test_callback)};
    auto ret = rule(arr);

    assert(1 == 1);

    return std::make_pair(value, *ret);
}

int main()
{

    {
        constexpr auto p1 = test();
        static_assert(p1.first == 1);
        static_assert(p1.second == 1);
    }
    {
        constexpr auto p1 = test2();
        static_assert(p1.first == 1);
        static_assert(p1.second == 1);
        {
            // NOTE: lambda 优化性确实更好
            constexpr auto p1 = test5();
            static_assert(p1.first == 1);
            static_assert(p1.second == 1);
        }
    }

    {
        static_assert(foo(1, 2, 3)() == 6); // NOLINT
    }
    {
        auto p1 = test3();
        assert(p1.first == 1);
        assert(p1.second == 1);
    }
    {
        // constexpr auto p1 = test4();
        auto p1 = test4();
        assert(p1.first == 1);
        assert(p1.second == 1);
    }
    {
        // NOTE: 是可能的。需要 constexpr
        constexpr auto p1 = test4();
        static_assert(p1.first == 1);
        static_assert(p1.second == 1);
    }
    {

        constexpr auto p1 = test6();
        static_assert(p1.first == 1);
        static_assert(p1.second == 2);
    }
    {

        constexpr auto p1 = test7();
        static_assert(p1.first == 1);
        static_assert(p1.second == 2);
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND