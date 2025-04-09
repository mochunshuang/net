#include <cassert>
#include <array>
#include <span>
#include <tuple>
#include <type_traits>

#include "../test_common/test_macro.hpp"

using OCTET = std::uint8_t;

// 解析结果类型
struct ParseResult
{
    bool success;    // NOLINT
    size_t consumed; // NOLINT

    constexpr explicit operator bool() const
    {
        return success;
    }
    constexpr bool operator!() const
    {
        return !success;
    }
};

template <typename T>
concept parser = (std::copy_constructible<T> || std::move_constructible<T>) &&
                 std::destructible<T> && (requires(T p, const std::span<const OCTET> &s) {
                     { p.check(s) } noexcept -> std::same_as<ParseResult>;
                 } || requires(T p, const OCTET &s) {
                     { p.check(s) } noexcept -> std::same_as<ParseResult>;
                 });

/**
3.1.  Concatenation:  Rule1 Rule2

   A rule can define a simple, ordered string of values (i.e., a
   concatenation of contiguous characters) by listing a sequence of rule
   names.  For example:

         foo         =  %x61           ; a

         bar         =  %x62           ; b

         mumble      =  foo bar foo

   So that the rule <mumble> matches the lowercase string "aba".
 */
// NOTE: 网络传输过滤的全是字符
template <parser... Rules>
    requires(sizeof...(Rules) >= 2)
class Concatenation
{

    std::tuple<Rules...> rules_;

  public:
    template <typename... Rs>
    explicit constexpr Concatenation(Rs &&...rs) noexcept
        : rules_{std::forward<Rs>(rs)...}
    {
    }

    Concatenation() = default;
    ~Concatenation() noexcept = default;
    Concatenation(const Concatenation &) = default;
    Concatenation(Concatenation &&) noexcept = default;
    Concatenation &operator=(const Concatenation &) = default;
    Concatenation &operator=(Concatenation &&) noexcept = default;

    template <typename... Ts>
        requires(sizeof...(Rules) == sizeof...(Ts))
    constexpr auto check(Ts &&...as) const noexcept
    {
        ParseResult final_result{.success = true, .consumed = 0};
        [&]<std::size_t... I>(std::index_sequence<I...>) constexpr noexcept {
            (([&] {
                 auto result = std::get<I>(rules_).check(std::forward<Ts>(as));
                 final_result.success &= result.success;
                 final_result.consumed += result.consumed;
             }()),
             ...);
        }(std::make_index_sequence<sizeof...(Rules)>{});
        return final_result;
    }
};

template <typename... Rs>
Concatenation(Rs &&...rs) // NOLINT
    ->Concatenation<std::decay_t<Rs>...>;

struct CheckA
{
    [[nodiscard]] ParseResult check(std::span<const OCTET> s) const noexcept
    {
        return {.success = !s.empty() && s[0] == 'a', .consumed = 1};
    }
};

struct CheckB
{
    [[nodiscard]] ParseResult check(std::span<const OCTET> s) const noexcept
    {
        return {.success = !s.empty() && s[0] == 'b', .consumed = 1};
    }
};

int main()
{
    TEST("AllRulesSucceed") = [] {
        Concatenation<CheckA, CheckB, CheckA> parser;
        std::array<OCTET, 3> input = {'a', 'b', 'a'};

        std::span<const OCTET> span(input);

        auto result =
            parser.check(span.subspan(0, 1), span.subspan(1, 1), span.subspan(2, 1));
        assert(result);
        assert(result.consumed == 3);

        assert(span.subspan(0, 1).size() == 1 && span.subspan(0, 1)[0] == 'a');
        assert(span.subspan(1, 1).size() == 1 && span.subspan(1, 1)[0] == 'b');
        assert(span.subspan(2, 1).size() == 1 && span.subspan(2, 1)[0] == 'a');
    };

    TEST("MiddleRuleFails") = [] {
        Concatenation<CheckA, CheckB, CheckA> parser;
        std::array<OCTET, 3> input = {'a', 'a', 'a'}; // 第二个字符不是 'b'
        std::span<const OCTET> span(input);
    };
}