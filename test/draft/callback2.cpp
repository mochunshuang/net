#include <array>
#include <cassert>
#include <cstddef>
#include <optional>
#include <utility>
#include <tuple>

// NOLINTBEGIN
template <size_t N>
struct ConstString
{
    char data[N]{};
    constexpr ConstString(const char (&str)[N])
    {
        for (size_t i = 0; i < N; ++i)
            data[i] = str[i];
    }
};

// 编译期解析上下文（不可变）
struct ParserCtx
{
    const char *input;
    size_t cur_index = 0;

    constexpr ParserCtx(const char *str) : input(str) {}
};

// 编译期空格规则
struct CompileTimeWhitespace
{
    constexpr auto operator()(ParserCtx &ctx) const noexcept -> std::optional<size_t>
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

// 编译期字符规则（带回调）
template <char C, typename Callback>
struct CompileTimeInsensitiveChar
{
    Callback callback;

    constexpr auto operator()(ParserCtx &ctx) const noexcept -> std::optional<size_t>
    {
        if (ctx.input[ctx.cur_index] == C)
        {
            callback(ctx.cur_index); // 编译期回调
            ctx.cur_index++;
            return 1;
        }
        return std::nullopt;
    }
};

// 编译期 sequence 组合子
template <typename... Rules>
struct CompileTimeSequence
{

    std::tuple<Rules...> rules;

    constexpr explicit CompileTimeSequence(Rules... rs) : rules(rs...) {}

    constexpr auto operator()(ParserCtx &ctx) const noexcept -> std::optional<size_t>
    {
        size_t total = 0;
        [[maybe_unused]] const size_t original_index = ctx.cur_index; // 消除警告

        auto apply_rule = [&](const auto &rule) -> bool {
            if (auto consumed = rule(ctx))
            {
                total += *consumed;
                return true;
            }
            return false;
        };

        const bool success = [&]<size_t... I>(std::index_sequence<I...>) {
            return (apply_rule(std::get<I>(rules)) && ...);
        }(std::make_index_sequence<sizeof...(Rules)>{});

        return success ? std::make_optional(total) : std::nullopt;
    }
};

template <typename... Rules>
struct Sequence
{
    std::tuple<Rules...> rules; 
    constexpr explicit Sequence(Rules... rs) : rules(rs...) {}
};

// 编译期解析入口（返回点号位置或空）
template <ConstString Str>
constexpr auto CompileTimeParse() -> std::array<size_t, 3>
{
    struct Callback
    {
        constexpr void operator()(size_t pos) noexcept
        {
            if (dot_count < 3)
                dots[dot_count++] = pos;
        }
        std::array<size_t, 3> dots{};
        size_t dot_count = 0;
    } callback;

    using DotRule = CompileTimeInsensitiveChar<'.', Callback &>;
    using WhitespaceRule = CompileTimeWhitespace;

    CompileTimeSequence<DotRule, WhitespaceRule, DotRule, WhitespaceRule, DotRule> parser{
        DotRule{callback}, WhitespaceRule{}, DotRule{callback}, WhitespaceRule{},
        DotRule{callback}};

    ParserCtx ctx{Str.data};
    if (parser(ctx).has_value())
    {
        return callback.dots;
    }
    return {}; // 解析失败返回空数组
}

// 编译期测试
constexpr bool RunCompileTimeTests()
{
    // 测试用例1: ". . ."
    constexpr auto test1 = CompileTimeParse<". . .">();
    static_assert(test1[0] == 0, "First dot position error");
    static_assert(test1[1] == 2, "Second dot position error");
    static_assert(test1[2] == 4, "Third dot position error");

    // 测试用例2: "..."
    constexpr auto test2 = CompileTimeParse<"...">();
    static_assert(test2[0] == 0, "First dot position error");
    static_assert(test2[1] == 0, "Second dot position error");
    static_assert(test2[2] == 0, "Third dot position error");

    return true;
}

// 确保编译期测试执行
static_assert(RunCompileTimeTests(), "Compile-time tests failed");

// 运行时包装（可选）
int main()
{
    // 运行时测试（直接使用编译期结果）
    constexpr auto rt_test = CompileTimeParse<". . .">();
    assert(rt_test[0] == 0 && rt_test[1] == 2 && rt_test[2] == 4);
    static_assert(rt_test[0] == 0 && rt_test[1] == 2 && rt_test[2] == 4);
    return 0;
}
// NOLINTEND