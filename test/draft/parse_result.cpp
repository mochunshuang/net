#include <cassert>
#include <expected>
#include <iostream>
#include <source_location>
#include <type_traits>

// NOLINTBEGIN

struct ParseError
{
    struct info
    {
        std::size_t consumed{};
        std::source_location location;
    };

    info first; // NOLINT
    info last;  // NOLINT

    explicit ParseError(std::size_t consumed,
                        std::source_location loc = std::source_location::current())
        : first{.consumed = consumed, .location = loc},
          last{.consumed = consumed, .location = loc}
    {
    }
    void add_context(std::size_t new_consumed,
                     std::source_location loc = std::source_location::current())
    {
        last.consumed = new_consumed;
        last.location = loc;
    }
};

struct ParseResult
{
    std::size_t consumed; // NOLINT
};

using value_t = ParseResult;
using error_t = ParseError;
using parse_result = std::expected<value_t, error_t>;
using parse_error = std::unexpected<error_t>;

// 模拟真实解析函数，根据输入返回成功或错误
parse_result parse_data(const std::string &input)
{
    // 成功案例：消耗全部输入
    if (!input.empty() && input[0] == 'V')
    {
        return value_t{input.length()};
    }

    // 错误案例1：空输入
    if (input.empty())
    {
        return std::unexpected(error_t(0)); // 使用默认source_location
    }

    // 错误案例2：消耗部分输入后失败
    for (size_t i = 0; i < input.size(); ++i)
    {
        if (!isdigit(input[i]))
        {
            static_assert(std::is_same_v<decltype(std::unexpected(error_t(i))),
                                         decltype(parse_error(error_t(0)))>);
            // 在发现非数字字符时失败，记录当前位置
            return std::unexpected(error_t(i));
        }
    }

    // 理论上不会执行到此处
    return parse_error(error_t(0));
}

#define CALL_ADD_CONTEXT(obj, consumed) obj.add_context(consumed)

int main()
{
    {
        // ConstructorInitializesMembers
        const std::size_t consumed = 5;
        const auto loc = std::source_location::current();
        const ParseError error(consumed, loc);

        // 检查consumed是否正确初始化
        assert(error.first.consumed == consumed);
        assert(error.last.consumed == consumed);

        // 检查source_location是否正确传递
        assert(error.first.location.line() == loc.line());
        assert(error.first.location.column() == loc.column());
        assert(error.first.location.file_name() == loc.file_name());
        assert(error.first.location.function_name() == loc.function_name());

        // 检查first和last的location是否相同
        assert(error.first.location.line() == error.last.location.line());
    }
    {
        // MemberInitialization
        const std::size_t consumed = 10;
        const ParseResult result{consumed};
        assert(result.consumed == consumed);
    }
    {
        // ExpectedWithValue
        const std::size_t consumed = 15;
        const parse_result result = ParseResult{consumed};

        assert(result.has_value());
        assert(result->consumed == consumed);
    }
    {
        // ExpectedWithError
        const std::size_t consumed = 20;
        const auto loc = std::source_location::current();
        const parse_result result = parse_error(ParseError(consumed, loc));
        static_assert(
            not std::is_same_v<std::remove_cvref_t<decltype(result)>, ParseResult>);

        assert(not result.has_value());
        assert(not result);
        const auto &error = result.error();
        assert(error.first.consumed == consumed);
        assert(error.last.consumed == consumed);
        assert(error.first.location.line() == loc.line());
    }
    {
        // DefaultSourceLocation
        const std::size_t consumed = 25;
        // 在下面这一行创建ParseError，使用默认的source_location
        const ParseError error(consumed);
        const auto call_line = __LINE__ - 1; // 上一行是构造error的行

        assert(error.first.location.line() == call_line);
        assert(error.last.location.line() == call_line);
    }
    {
        // FullParsingSuccess
        const std::string valid_input = "ValidData";
        auto result = parse_data(valid_input);

        assert(result.has_value());
        assert(result);
        assert(result->consumed == valid_input.length());
    }
    {
        // EmptyInputError
        auto result = parse_data("");

        assert(not result.has_value());
        assert(not result);
        const auto &error = result.error();
        assert(error.first.consumed == 0);
        // 验证错误位置指向parse_data函数内的返回行
        std::cout << "fun: " << error.first.location.function_name() << '\n';
        assert(std::string(error.first.location.function_name()).find("parse_data") !=
               std::string::npos);
    }
    {
        // PartialParseFailure
        const std::string invalid_input = "123A56"; // 第四个字符'A'非法
        auto result = parse_data(invalid_input);

        assert(not result.has_value());
        const auto &error = result.error();
        // 应当消耗前3个合法字符
        assert(error.first.consumed == 3);

        // 验证错误位置在parse_data循环内的返回行
        assert(std::string(error.first.location.function_name()).find("parse_data") !=
               std::string::npos);
    }
    {
        // ErrorPropagation
        auto parse_number = [](const std::string &input) -> parse_result {
            if (input.empty() || !isdigit(input[0]))
            {
                // 记录错误发生位置
                return parse_error(ParseError(0, std::source_location::current()));
            }
            return ParseResult{1}; // 仅消耗第一个字符
        };

        auto result = parse_number("X");
        assert(not result.has_value());

        // 验证错误信息包含lambda内的构造位置
        const auto &error = result.error();
        std::cout << "fun: " << error.first.location.function_name() << '\n';
        assert(std::string(error.first.location.function_name())
                   .find("const std::string&") != std::string::npos);
    }
    {
        // ErrorPropagationUpdate
        // 内层错误
        const auto inner_line = __LINE__ + 1;
        ParseError inner_error(10);

        // 外层处理（模拟添加上下文）
        const auto outer_line = __LINE__ + 1;
        inner_error.add_context(20);

        // 验证first保持内层信息
        assert(inner_error.first.consumed == 10);
        assert(inner_error.first.location.line() == inner_line);

        // 验证last更新为外层信息
        assert(inner_error.last.consumed == 20);
        assert(inner_error.last.location.line() == outer_line);
    }
    {
        // MultiLayerError
        // 最内层错误
        ParseError error(5);
        const auto first_line = __LINE__ - 1;

        // 中间层包装
        error.add_context(15);
        const auto mid_line = __LINE__ - 1;

        // 最外层包装
        error.add_context(25, std::source_location::current());
        const auto outer_line = __LINE__ - 1;

        // 验证first始终不变
        assert(error.first.consumed == 5);
        assert(error.first.location.line() == first_line);

        // 验证last持续更新
        assert(error.last.consumed == 25);
        assert(error.last.location.line() == outer_line);

        // 验证中间层信息被覆盖
        assert(error.last.location.line() != mid_line);
    }
    {
        ParseError err(0);

        // 直接调用
        err.add_context(10); // location 会指向这一行

        // 通过宏调用
        CALL_ADD_CONTEXT(err, 20); // location 会指向这一行，而不是宏定义处
        const auto outer_line = __LINE__ - 1;
        assert(err.last.location.line() == outer_line);
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND