// NOLINTBEGIN

#include <expected>
#include <source_location>
#include <string>
#include <iostream>

struct ErrorInfo
{
    int code;
    std::string message;
    std::source_location location;

    ErrorInfo(int c, std::string m,
              std::source_location loc = std::source_location::current())
        : code(c), message(std::move(m)), location(loc)
    {
    }
};

// 示例1：捕获错误发生的位置（函数内部）
std::expected<int, ErrorInfo> compute(int x)
{
    if (x < 0)
    {
        return std::unexpected(ErrorInfo{1, "x必须非负"}); // 错误位置为return语句处
    }
    return x * 2;
}

// 示例2：捕获调用点的位置（参数校验）
std::expected<int, ErrorInfo> safe_compute(
    int x, std::source_location loc = std::source_location::current())
{
    if (x < 0)
    {
        return std::unexpected(ErrorInfo{2, "参数无效", loc}); // 错误位置为调用处
    }
    return x * 2;
}

int main()
{
    // 测试内部错误位置捕获
    auto result1 = compute(-5);
    if (!result1)
    {
        const auto &err = result1.error();
        std::cerr << "错误1: " << err.message << "\n位置: " << err.location.file_name()
                  << ":" << err.location.line() << "\n";
    }

    // 测试调用点位置捕获
    auto result2 = safe_compute(-3);
    if (!result2)
    {
        const auto &err = result2.error();
        std::cerr << "错误2: " << err.message << "\n位置: " << err.location.file_name()
                  << ":" << err.location.line() << "\n";
    }

    return 0;
}
// NOLINTEND