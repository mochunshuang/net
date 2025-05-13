
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <span>
#include "__product_type.hpp"
#include <tuple>
#include <type_traits>

using octet = uint8_t;
using octet_param_in = const octet &;
using span_param_in = const std::span<const octet> &;
static constexpr auto k_max_octet_value = std::numeric_limits<octet>::max();
static constexpr auto k_min_octet_value = std::numeric_limits<octet>::min();

struct rule_t
{
};
template <class Rule>
concept is_rule = std::derived_from<typename Rule::rule_concept, rule_t>;
template <class Rule>
concept enable_rule = is_rule<Rule>;
template <class Rule>
concept ruleer = enable_rule<std::remove_cvref_t<Rule>> &&
                 std::move_constructible<std::remove_cvref_t<Rule>> && // movable
                 std::constructible_from<std::remove_cvref_t<Rule>,
                                         Rule>; // copyable

struct parse_result
{
    bool success{false};
    std::size_t consumed{0};
};

struct parse_state
{
  private:
    std::span<const octet> input_;
    std::size_t index_{0};

  public:
    explicit parse_state(std::span<const octet> in, std::size_t idx = 0) noexcept
        : input_{in}, index_{idx}
    {
    }
    [[nodiscard]] bool done() const noexcept
    {
        return input_.size() == index_;
    }
    auto &index() noexcept
    {
        return index_;
    }
};

struct parser_ctx
{
    parse_state state;
    parse_result r;
};

namespace terminal_values
{
    static_assert(sizeof(char) == sizeof(octet));
    static_assert(std::is_same_v<octet, unsigned char>);
    static_assert(not std::is_same_v<char, unsigned char>);

    struct tolower_t
    {
        constexpr char operator()(octet_param_in c) const noexcept
        {
            // NOLINTNEXTLINE
            return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
        }
    };
    inline constexpr tolower_t tolower{}; // NOLINT
    static_assert(tolower('A') == 'a');
    static_assert(tolower('a') == 'a');

    struct char_t
    {
        using rule_concept = rule_t;
        explicit char_t(char c) noexcept : c_(c)
        {
            static_assert(std::numeric_limits<char>::max() < k_max_octet_value);
            assert(c >= k_min_octet_value);
        }
        auto operator()(octet_param_in c) const
        {
            return tolower(c) == tolower(static_cast<octet>(c_));
        }

      private:
        char c_;
    };

    struct char_sensitive_t
    {
        using rule_concept = rule_t;
        explicit char_sensitive_t(char c) noexcept : c_(c)
        {
            static_assert(std::numeric_limits<char>::max() < k_max_octet_value);
            assert(c >= k_min_octet_value);
        }
        auto operator()(octet_param_in c) const
        {
            return c == static_cast<octet>(c_);
        }

      private:
        char c_;
    };

}; // namespace terminal_values

/**
       foo         =  %x61           ; a

       bar         =  %x62           ; b

       mumble      =  foo bar foo
    */
template <ruleer... R>
struct concatenation_rule
{
  private:
    std::tuple<R...> impl_;

  public:
    explicit concatenation_rule(R &&...r) : impl_{std::forward<R>(r)...} {}

    auto operator()(span_param_in sp) const
    {
        parser_ctx ctx(parse_state{sp});
        for (std::size_t i{0}; i < sizeof...(R); ++i)
        {

            ctx.state.index()++;
        }

        return ctx;
    }
};
template <typename... R> // NOLINTNEXTLINE
concatenation_rule(R &&...r) -> concatenation_rule<std::remove_cvref_t<R>...>;

/**
 * @brief 规则的集成和组合，规则间的依赖。 工作进度。通配符。 数据+算法。顺序。进度单位。
 *
 * @return int
 */

int main()
{
    {
        constexpr std::span<const octet> k_input;
        static_assert(k_input.size() == 0);
    }
    {
        auto c = concatenation_rule(terminal_values::char_t('a'),
                                    terminal_values::char_t('b'));
    }
    std::cout << "main done\n";
    return 0;
}