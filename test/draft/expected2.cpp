#include <expected>
#include <iostream>
#include <string>

template <typename T>
struct my_type
{
    explicit my_type(const char * /*unused*/) noexcept {} // 假设有构造函数接受字符串
};

template <typename T>
struct result : public std::expected<my_type<T>, int>
{
    using std::expected<my_type<T>, int>::expected;
};

// 测试函数返回result<std::string>
auto test(int v) -> result<std::string>
{
    if (v == 0)
    {
        return std::unexpected(v); // 错误情况返回unexpected
    }
    return my_type<std::string>("123"); // 正确情况返回my_type
}

int main()
{
    std::cout << "main done\n";
    return 0;
}