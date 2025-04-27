#include <cassert>
#include <optional>
#include <array>
#include <tuple>

// NOLINTBEGIN
namespace detail
{
    struct parser_ctx
    {
        const char *input;
        size_t cur_index = 0;
    };

    template <typename T>
    using consumed_result = std::optional<size_t>;
} // namespace detail

// 新增：匹配任意数量空格的规则
struct Whitespace
{
    constexpr auto operator()(detail::parser_ctx &ctx) const noexcept
        -> detail::consumed_result<size_t>
    {
        size_t count = 0;
        while (ctx.input[ctx.cur_index] == ' ')
        {
            ctx.cur_index++;
            count++;
        }
        return count > 0 ? std::make_optional(count) : std::nullopt;
    }
};

template <char C, typename Callback = void>
struct InsensitiveChar
{
    Callback callback;

    constexpr auto operator()(detail::parser_ctx &ctx) const noexcept
        -> detail::consumed_result<size_t>
    {
        if (ctx.input[ctx.cur_index] == C)
        {
            if constexpr (!std::is_void_v<Callback>)
            {
                callback(ctx.cur_index);
            }
            ctx.cur_index += 1;
            return 1;
        }
        return std::nullopt;
    }
};

template <typename... Rules>
struct sequence
{
    std::tuple<Rules...> rules;

    constexpr sequence(Rules... rs) : rules(rs...) {}

    constexpr auto operator()(detail::parser_ctx &ctx) const noexcept
        -> detail::consumed_result<size_t>
    {
        size_t total = 0;
        size_t original_index = ctx.cur_index;

        auto apply = [&](const auto &rule) -> bool {
            if (auto consumed = rule(ctx))
            {
                total += *consumed;
                return true;
            }
            return false;
        };

        bool success = [&]<size_t... I>(std::index_sequence<I...>) {
            return (apply(std::get<I>(rules)) && ...);
        }(std::make_index_sequence<sizeof...(Rules)>{});

        return success ? std::make_optional(total) : std::nullopt;
    }
};

// 测试代码
struct Result
{
    std::array<size_t, 3> dot_indices{};
    size_t count = 0;
};

int main()
{
    Result result;
    auto callback = [&](size_t pos) {
        if (result.count < 3)
            result.dot_indices[result.count++] = pos;
    };

    using DotRule = InsensitiveChar<'.', decltype(callback) &>;
    using WhitespaceRule = Whitespace;

    // 修改规则：点号之间允许空格
    sequence<DotRule, WhitespaceRule, DotRule, WhitespaceRule, DotRule> rule{
        DotRule{callback}, WhitespaceRule{}, DotRule{callback}, WhitespaceRule{},
        DotRule{callback}};

    // 输入 ". . ."（点号位于索引0, 2, 4）
    detail::parser_ctx ctx{.input = ". . ."};
    auto ret = rule(ctx);

    assert(ret.has_value() && *ret == 5); // 总消费5个字符（3点号+2空格）
    assert(result.dot_indices[0] == 0);
    assert(result.dot_indices[1] == 2);
    assert(result.dot_indices[2] == 4);
    assert(result.count == 3);

    return 0;
}
// NOLINTEND