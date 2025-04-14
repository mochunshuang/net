#include <atomic>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <expected>
#include <iostream>
#include <type_traits>
#include <array>
#include <utility>

// NOLINTBEGIN
namespace __detail
{
    template <typename T>
    inline constexpr bool is_expected = false;
    template <typename V, typename E>
    inline constexpr bool is_expected<std::expected<V, E>> = true;
} // namespace __detail

template <typename T>
concept is_expected_specialization = __detail::is_expected<T>;

template <typename R>
concept abnf_result = is_expected_specialization<R> || std::same_as<R, bool>;

template <typename V, typename E>
struct result_builder
{
    using value_type = V;
    using error_type = E;
    using result_type = std::expected<value_type, error_type>;

    template <typename... Args>
        requires requires {
            result_type(std::in_place, std::forward<Args>(std::declval<Args>())...);
        }
    static constexpr auto success(Args &&...args) noexcept
    {
        return result_type(std::in_place, std::forward<Args>(args)...);
    }

    template <typename Err>
    static constexpr auto fail(Err &&err) noexcept
    {
        return result_type(std::unexpected(std::forward<Err>(err)));
    }
};

struct span
{
    std::size_t start{}; // NOLINT
    std::size_t end{};   // NOLINT
    bool operator==(const span &) const = default;
};

template <class From, class To>
concept decays_to = std::same_as<std::decay_t<From>, To>;

template <std::size_t N>
struct spans
{
    using value_type = span;
    using reference = value_type &;

  private:
    std::array<value_type, N> spans_;

  public:
    // 修正点1：正确初始化数组
    template <typename... Args>
        requires(sizeof...(Args) == N) &&
                (std::is_constructible_v<value_type, Args> && ...)
    constexpr explicit spans(Args &&...args) noexcept
        : spans_{std::forward<Args>(args)...} // 这里默认就是初始化列表了，不用{}包括
    {
    }

    template <std::size_t idx, decays_to<spans> Self>
        requires(idx < N)
    constexpr decltype(auto) get(this Self &&self) noexcept
    {
        return std::forward_like<Self>(std::forward<Self>(self).spans_[idx]);
    }
};

template <std::size_t N>
struct result
{
    std::size_t count_;
    spans<N> spans_;

    template <typename... Args>
        requires(sizeof...(Args) == N) && (std::is_constructible_v<span, Args> && ...)
    constexpr explicit result(std::size_t count, Args &&...e) noexcept
        : count_{count}, spans_{std::forward<Args>(e)...} // 直接传递参数包
    {
    }

    constexpr span &operator[](std::size_t index)
    {
        return spans_[index];
    }
    [[nodiscard]] consteval std::size_t size() const
    {
        return N;
    }

    template <std::size_t I, decays_to<result> Self>
        requires(I < N)
    constexpr decltype(auto) get(this Self &&self) noexcept
    {
        return std::forward_like<Self>(std::forward<Self>(self).spans_.template get<I>());
    }

    constexpr auto count() noexcept
    {
        return count_;
    }
};

constexpr abnf_result auto dec_octet()
{
    using builder = result_builder<result<1>, int>;
    return builder::success(1, span{0, 3}); // 正确传递单个 span
}

constexpr abnf_result auto dec_octet2()
{
    using builder = result_builder<result<2>, int>;
    return builder::success(2, span{0, 3}, span{4, 7});
}
constexpr abnf_result auto dec_octet3()
{
    using builder = result_builder<result<3>, int>;
    return builder::success(2, span{0, 3}, span{}, span{4, 7});
}

std::atomic<int> construction_count{0};
std::atomic<int> deconstruction_count{0};
constexpr abnf_result auto test()
{
    struct my_type
    {
        my_type() noexcept
        {
            construction_count++;
            std::cout << "my_type construction_count: " << construction_count << '\n';
        }
        my_type(const my_type &) = default;
        my_type(my_type &&) = default;
        my_type &operator=(const my_type &) = default;
        my_type &operator=(my_type &&) = default;
        ~my_type() noexcept
        {
            deconstruction_count++;
            std::cout << "my_type deconstruction_count: " << deconstruction_count << '\n';
        }
    };
    using builder = result_builder<my_type, int>;
    return builder::success(my_type{});
}

// NOTE: 下面直接编译期报错
//  constexpr abnf_result auto dec_octet_error()
//  {
//      using builder = result_builder<result<2>, int>;
//      return builder::success(2, span{0, 3});
//  }

namespace std
{
    // for structured_binding
    template <std::size_t N>
    struct tuple_size<::result<N>> // NOLINT
        : ::std::integral_constant<std::size_t, N>
    {
    };

    template <std::size_t N, ::std::size_t I>
    struct tuple_element<I, ::result<N>> // NOLINT
    {
        using type =
            ::std::decay_t<decltype(::std::declval<::result<N>>().template get<I>())>;
    };

}; // namespace std

int main()
{
    constexpr auto res = dec_octet();
    static_assert(res->spans_.get<0>().start == 0);
    {
        auto res = dec_octet();
        auto v = res.value();
        auto [a] = v;
        assert(a.start == 0);
        assert(a.end == 3);
        {
            auto [a] = dec_octet().value();
            assert(a.start == 0);
            assert(a.end == 3);
        }
    }
    {
        auto res = dec_octet3();
        auto v = res.value();
        auto [a, b, c] = v;
        auto a0 = v.get<0>();
        assert(a == a0);
    }
    {
        struct A
        {
            std::size_t count{};

            auto getCount() const noexcept
            {
                return count;
            }
        };
        A aa;
        {
            auto v = aa.getCount();
        }
        aa.count = 1;
        auto v = std::move(aa.count);
        assert(v == 1);
        assert(aa.count == 1);
        std::cout << "aa.count: " << aa.count << '\n'; // 1

        // NOTE: 对基本类型 move 没有用。 没有意义
        for (int i = 0; i < 100; ++i)
        {
            auto v = std::move(aa.count);
            assert(aa.count == 1);
            assert(v == 1);
        }
    }
    {
        // NOTE: 只能保证一次构造，不能保证一次析构
        // NOTE:
        auto &&v = test();
    }
    return 0;
}
// NOLINTEND