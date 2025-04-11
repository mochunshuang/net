// NOLINTBEGIN
#include <expected>
#include <source_location>
#include <iostream>
#include <stdexcept>

// 错误信息结构体（无堆内存分配）
struct ErrorInfo
{
    struct Context
    {
        std::source_location location; // 源代码位置
        const char *message;           // 静态字符串描述（避免堆分配)
        // NOTE: message 只能是字面量
    };

    Context inner; // 最内层错误（原始错误点）
    Context outer; // 最外层错误（最终处理点）
    int code;      // 错误码

    // 构造函数：初始化内层和outer为同一位置
    ErrorInfo(int c, const char *msg,
              std::source_location loc = std::source_location::current())
        : code(c), inner{loc, msg}, outer{loc, msg}
    {
    }
};

// 示例函数：底层操作可能引发错误
std::expected<int, ErrorInfo> compute(int x)
{
    if (x < 0)
    {
        return std::unexpected(ErrorInfo{1, "Negative value in compute"}); // 记录内层错误
    }
    return x * 2;
}

// 示例函数：中层传递错误并更新外层信息
std::expected<int, ErrorInfo> process(int x)
{
    auto result = compute(x);
    if (!result)
    {
        ErrorInfo err = result.error();
        err.outer = {std::source_location::current(), "Process failed"}; // 更新外层错误
        return std::unexpected(err);
    }
    return result;
}

// 示例函数：顶层处理错误并记录最终外层信息
std::expected<int, ErrorInfo> validate(int x)
{
    auto result = process(x);
    if (!result)
    {
        ErrorInfo err = result.error();
        err.outer = {std::source_location::current(),
                     "Validation failed"}; // 更新外层错误
        return std::unexpected(err);
    }
    return result;
}

int main()
{
    auto result = validate(-5); // 触发错误
    if (!result)
    {
        const auto &err = result.error();
        std::cerr << "错误码: " << err.code << "\n";
        std::cerr << "原始错误: " << err.inner.message
                  << "\n  位置: " << err.inner.location.file_name() << ":"
                  << err.inner.location.line() << "\n";
        std::cerr << "最终处理: " << err.outer.message
                  << "\n  位置: " << err.outer.location.file_name() << ":"
                  << err.outer.location.line() << "\n";
    }
    {
        std::runtime_error{std::string("1")};
    }
    return 0;
}

// NOLINTEND