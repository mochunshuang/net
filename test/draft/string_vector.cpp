#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// NOLINTBEGIN

// 将 std::vector<char> 高效转换为 std::string
std::string vectorToString(std::vector<char> &&vec)
{
    std::cout << ">>>> 转换前的 vec 大小: " << vec.size() << '\n';
    auto str = std::string(std::make_move_iterator(vec.begin()),
                           std::make_move_iterator(vec.end()));
    assert(vec.size() != 0);
    std::cout << ">>>> 转换后的 vec 大小: " << vec.size() << '\n';
    return str;
}

// 将 std::string 高效转换为 std::vector<char>
std::vector<char> stringToVector(std::string &&str)
{
    std::cout << ">>>> 转换前的 str 大小: " << str.size() << '\n';
    // 预分配足够的内存，避免后续重新分配
    std::vector<char> vec;
    vec.reserve(str.size() + 1); // +1 为了包含字符串结束符 '\0'

    // 使用 std::move 避免拷贝
    vec.assign(std::make_move_iterator(str.begin()), std::make_move_iterator(str.end()));

    // 添加字符串结束符（可选，根据需要）
    vec.emplace_back('\0');

    assert(str.size() != 0);
    std::cout << ">>>> 转换后的 str 大小: " << str.size() << '\n';
    return vec;
}

int main()
{
    // 示例1: std::string 转 std::vector<char>
    std::string str = "Hello, World!";
    std::vector<char> vec = stringToVector(std::move(str));

    std::cout << "转换后的 vector 内容: ";
    for (char c : vec)
    {
        std::cout << c;
    }
    std::cout << std::endl;

    // 示例2: std::vector<char> 转 std::string
    std::vector<char> vec2 = {'H', 'e', 'l', 'l', 'o'};
    std::string str2 = vectorToString(std::move(vec2));

    std::cout << "转换后的 string 内容: " << str2 << std::endl;

    // NOTE: 平凡类型，默认是 按值语义处理的

    return 0;
}
// NOLINTEND