#include <cassert>
#include <iostream>
#include <optional>
#include <span>
#include <array>

/**
 * @brief OCTET          =  %x00-FF
 *                          ; 8 bits of data
 */
using octet = std::uint8_t;

using octets_view = std::span<const octet>;
using octets_view_in = const octets_view &;
using octet_in = const octet &;

using consumed_result = std::optional<std::size_t>;
constexpr auto make_consumed_result(size_t v) noexcept -> consumed_result // NOLINT
{
    return {v};
}
struct rule_t
{
};

struct parser_ctx
{
    const octets_view root_span;   // NOLINT
    std::size_t cur_index{};       // NOLINT
    const std::size_t end_index{}; // NOLINT

    [[nodiscard]] constexpr auto remain() const noexcept
    {
        return end_index - cur_index;
    }
    [[nodiscard]] constexpr auto done() const noexcept
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
using parser_ctx_ref = parser_ctx &;

template <octet Char>
struct SimpleRule
{
  public:
    using rule_concept = rule_t;

    struct __type
    {
        using domain = SimpleRule;
        octets_view value;
    };
    using result_type = __type;

    static constexpr auto operator()(parser_ctx_ref ctx) noexcept -> consumed_result
    {
        auto ret = ctx.root_span[ctx.cur_index] == Char;
        return ret ? make_consumed_result(1) : std::nullopt;
    }
    static constexpr auto parse(parser_ctx_ref ctx) noexcept -> std::optional<result_type>
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

constexpr inline auto a_rule = SimpleRule<'a'>{};
constexpr inline auto b_rule = SimpleRule<'b'>{};

// 1. 统一校验函数签名（与解析规则整合）
using ValidatorFunc = bool (*)(parser_ctx_ref);

// 2. 增强字段注册结构
struct FieldName
{
    std::string_view name;
    ValidatorFunc validator; // 绑定解析规则
};

template <typename Rule>
constexpr bool validate_rule(parser_ctx_ref ctx)
{
    return Rule::parse(ctx).has_value();
}

constexpr bool validate(octets_view_in span)
{
    return true;
}

// 4. 注册表构建（示例）
static constexpr auto registration = [] consteval {
    return std::array{FieldName{
                          .name = "Accept",
                          .validator = &validate_rule<SimpleRule<'a'>> // 绑定字母校验规则
                      },
                      FieldName{
                          .name = "Content-Length",

                          .validator = &validate_rule<SimpleRule<'b'>> // 绑定数字校验规则
                      }};
}();

int main()
{
    {
        constexpr auto a = registration[0];
        constexpr auto input = std::array<octet, 1>{'a'};
        octets_view span = std::span{input};
        auto cxt = make_parser_ctx(span);
        assert(a.validator(cxt));
    }
    {
        constexpr auto b = registration[1];
        constexpr auto input = std::array<octet, 1>{'b'};
        octets_view span = std::span{input};
        auto cxt = make_parser_ctx(span);
        assert(b.validator(cxt));
    }
    std::cout << "main done\n";
    return 0;
}