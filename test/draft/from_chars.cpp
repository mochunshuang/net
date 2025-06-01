#include <array>
#include <cassert>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <optional>
#include <string_view>
#include <system_error>
#include <iostream>
#include <span>

// NOLINTBEGIN

int hex_string_to_decimal(std::string_view hex_str)
{
    int result;
    auto [ptr, ec] =
        std::from_chars(hex_str.data(), hex_str.data() + hex_str.size(), result, 16);

    if (ec != std::errc())
    {
        // 处理错误，例如无效的十六进制字符串
        throw std::runtime_error("Invalid hexadecimal string");
    }

    return result;
}

constexpr auto hex_string_to_decimal(auto span) noexcept
    // ->std::expected<size_t,std::error_code>
    -> std::optional<size_t>
{
    static_assert(sizeof(std::error_code) == 16);
    static_assert(sizeof(std::size_t) == 8);
    static_assert(sizeof(std::expected<size_t, std::error_code>) == 24);
    static_assert(sizeof(std::optional<size_t>) == 16); // NOTE: optional 开销更小

    auto span_ = std::span{(char *)(span.data()), span.size()};
    size_t result = 0;
    auto [ptr, ec] =
        std::from_chars(span_.data(), span_.data() + span_.size(), result, 16);
    if (ec != std::errc())
        return std::nullopt;
    return result;
}

// 编译期十六进制字符转数值
constexpr std::optional<uint8_t> hex_char_to_value(std::uint8_t c) noexcept
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return std::nullopt;
}
// 编译期十六进制转换函数 (支持任意字符类型)
template <typename T>
constexpr auto hex_span_to_decimal(const T &data) noexcept -> std::optional<size_t>
{
    using CharType = std::remove_cv_t<typename T::value_type>;
    static_assert(sizeof(CharType) == 1, "Only byte-sized types supported");

    size_t value = 0;
    bool valid_digit_found = false;

    for (size_t i = 0; i < data.size(); ++i)
    {
        const auto digit = hex_char_to_value(data[i]);
        if (!digit)
        {
            if (!valid_digit_found)
                return std::nullopt;
            break;
        }
        if (value > (std::numeric_limits<size_t>::max() >> 4))
        {
            return std::nullopt;
        }
        value = (value << 4) | *digit;
        valid_digit_found = true;
    }

    if (!valid_digit_found)
        return std::nullopt;
    return value;
}

int main()
{
    try
    {
        // 假设 chunk_size 是一个包含十六进制数字的字符串视图
        std::string_view chunk_size = "1A3F"; // 示例输入

        int size = hex_string_to_decimal(chunk_size);
        std::cout << "Decimal size: " << size << std::endl;

        assert(size == 6719);

        static_assert(0x1a3f == 6719);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    {
        // span
        std::array<std::uint8_t, 4> data = {'1', 'A', '3', 'F'};
        auto span = std::span{(char *)(data.data()), data.size()};
        size_t result = 0;
        auto [ptr, ec] =
            std::from_chars(span.data(), span.data() + span.size(), result, 16);

        if (ec != std::errc())
        {
            // 处理错误，例如无效的十六进制字符串
            throw std::runtime_error("Invalid hexadecimal string");
        }
        assert(result == 6719);
    }
    {
        constexpr std::array<std::uint8_t, 4> data = {'1', 'A', '3', 'F'};
        assert(hex_string_to_decimal(data) == 6719); // NOTE: 无法 静态断言
    }
    {
        constexpr std::array<std::uint8_t, 4> data = {'1', 'A', '3', 'F'};
        static_assert(hex_span_to_decimal(std::span{data}) == 6719); // NOTE: 静态断言
        static_assert(hex_span_to_decimal(data) == 6719);

        {
            static_assert(hex_span_to_decimal(
                              std::array<std::uint8_t, 4>{'0', '0', '0', '0'}) == 0);
        }
    }
    return 0;
}
// NOLINTEND