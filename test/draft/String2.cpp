#include <cstdint>
#include <array>
#include <algorithm>
#include <iostream>

using octet = std::uint8_t;

// 用于包装字符串字面量的模板类
template <size_t N>
struct FixedString
{
    static constexpr size_t size = N;                    // NOLINT
    char value[N]{};                                     // NOLINT
    constexpr FixedString(const char (&str)[N]) noexcept // NOLINT
    {
        std::copy_n(str, N, value);
    }
};

// 主模板定义
template <FixedString Str, bool Insensitive = true>
struct String
{
    // 计算字符串长度（排除终止符）
    constexpr static size_t k_count = decltype(Str)::size - 1;

    // 编译时生成octet数组
    constexpr static auto k_string = []() consteval {
        std::array<octet, k_count> arr{};
        for (size_t i = 0; i < k_count; ++i)
        {
            arr[i] = static_cast<octet>(Str.value[i]);
        }
        return arr;
    }();

    // 测试用成员函数
    [[nodiscard]] constexpr auto get() const noexcept -> const auto &
    {
        return k_string;
    }
};

int main()
{
    // 直接使用字符串字面量初始化
    constexpr auto s = String<"abc">{};

    // 验证转换结果
    static_assert(s.get()[0] == 'a');
    static_assert(s.get()[1] == 'b');
    static_assert(s.get()[2] == 'c');
    static_assert(s.get().size() == 3);

    std::cout << "Success!\n";
    return 0;
}