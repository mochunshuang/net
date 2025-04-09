
#include <cassert>
#include <cmath>
#include <iostream>

#include <expected>
#include <string>
#include <string_view>

// NOLINTBEGIN

std::expected<int, std::string> parseInput(const std::string &input)
{
    try
    {
        return std::stoi(input);
    }
    catch (...)
    {
        return std::unexpected("Invalid integer input");
    }
}

std::expected<double, std::string> sqrtIfPositive(int x)
{
    if (x < 0)
    {
        return std::unexpected("Negative value");
    }
    return std::sqrt(x);
}

std::expected<int, std::string> getValue(bool success)
{
    // NOTE: 编译错误
    //  return success == true ? 42 : std::unexpected<std::string>("Failure");
    return success ? std::expected<int, std::string>(42) : std::unexpected("Failure");
}

std::expected<int, std::string> tryFirstOption()
{
    return std::unexpected("First failed");
}

std::expected<int, std::string> trySecondOption()
{
    return 42;
}

std::expected<int, std::string> tryThirdOption()
{
    return std::unexpected("Third failed");
}

std::expected<int, int> mightFail(bool success)
{
    // return success ? 42 : std::unexpected(404);
    if (success)
        return 42;
    return std::unexpected(404);
}

std::expected<int, std::string> appendError(std::expected<int, std::string> e,
                                            const std::string &msg)
{
    if (!e)
        return std::unexpected(e.error() + "-> " + msg);
    return e;
}

int main()
{
    // NOTE: and_then 没用错误处理
    {
        auto result = parseInput("16")
                          .and_then(sqrtIfPositive)
                          .and_then([](double x) -> std::expected<double, std::string> {
                              return x * 2;
                          });

        if (result)
        {
            std::cout << "Result: " << *result << std::endl;
        }
        else
        {
            std::cerr << "Error: " << result.error() << std::endl;
        }

        // 错误路径示例
        auto errorResult = parseInput("-16").and_then(sqrtIfPositive);
        if (!errorResult)
        {
            std::cerr << "Error path: " << errorResult.error() << std::endl;
        }
    }
    // NOTE: transform 值转化
    {
        std::cerr << "\ntransform: " << std::endl;
        // 成功路径
        auto result1 =
            getValue(true)
                .transform([](int x) { return x * 1.5; })
                .transform([](double x) { return std::to_string(x) + " is the answer"; });

        // 错误路径
        auto result2 = getValue(false).transform([](int x) { return x * 1.5; });

        std::cout << "Success: " << *result1 << std::endl;
        std::cout << "Error: " << result2.error() << std::endl;
    }

    {
        std::cerr << "\nor_else: " << std::endl;
        // 尝试多个选项直到成功,成功后，后面的都不会调用
        auto result = tryFirstOption()
                          .or_else([](std::string &&s) {
                              assert(s == std::string_view("First failed"));
                              return trySecondOption();
                          })
                          .or_else([](auto) { return tryThirdOption(); });

        if (result)
        {
            std::cout << "Success with value: " << *result << std::endl;
        }

        // 全部失败的情况
        auto allFailed = tryFirstOption().or_else([](auto &&e) {
            assert(e == std::string_view("First failed"));
            return tryThirdOption();
        });

        if (!allFailed)
        {
            std::cerr << "All failed with: " << allFailed.error() << std::endl;
        }
    }

    // NOTE: transform_error - 错误转换
    {
        // 转换错误代码为描述性消息
        auto result1 = mightFail(false).transform_error(
            [](int code) { return std::format("Error code {} occurred", code); });

        // 不影响成功路径
        auto result2 = mightFail(true).transform_error(
            [](int code) { return std::format("Error code {} occurred", code); });

        if (!result1)
        {
            std::cout << "Transformed error: " << result1.error() << std::endl;
        }

        if (result2)
        {
            std::cout << "Success value: " << *result2 << std::endl;
        }
    }
    {
        auto result = std::expected<int, std::string>(std::unexpected("Error1"));
        result = appendError(result, "Error2");
        std::cout << "Combined error: " << result.error() << std::endl;
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND