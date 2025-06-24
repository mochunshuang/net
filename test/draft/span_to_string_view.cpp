#include <cstdint>
#include <iostream>

#include <span>
#include <string_view>
#include <array>
#include <bit>

// NOLINTBEGIN

using span_param_in = const std::span<const uint8_t> &;

constexpr auto equal_value(span_param_in a, const std::string_view &b) noexcept
{
    const auto k_size = a.size();
    if (k_size != b.size())
        return false;
    for (std::size_t i = 0; i < k_size; ++i)
    {
        if (a[i] != static_cast<uint8_t>(b[i]))
            return false;
    }
    return true;
}

// 将 std::span<const uint8_t> 转换为 std::string_view
inline std::string_view to_string_view(span_param_in data) noexcept
{
    return {reinterpret_cast<const char *>(data.data()), // 安全类型转换
            data.size()};
}

// 将 std::string_view 转换为 std::span<const uint8_t>
inline std::span<const uint8_t> to_byte_span(std::string_view str) noexcept
{
    return {reinterpret_cast<const uint8_t *>(str.data()), // 安全类型转换
            str.size()};
}

int main()
{
    constexpr auto test = []() constexpr {
        unsigned char buffer[] = {72, 101, 108, 108, 111}; // "Hello"
        auto sp = std::span{buffer};
        std::string_view str = "Hello";
        return equal_value(sp, str);
    };
    static_assert(test());

    {
        // 示例 1: span -> string_view
        constexpr std::array<uint8_t, 5> hello_bytes = {'H', 'e', 'l', 'l', 'o'};
        std::span sp_bytes = std::span{hello_bytes};

        // 转换并验证
        std::string_view sv_converted = to_string_view(sp_bytes);
        std::cout << "Span to string_view: " << sv_converted << "\n";
        std::cout << "Conversion valid: " << std::boolalpha << (sv_converted == "Hello")
                  << "\n\n";

        // 示例 2: string_view -> span
        std::string_view original_sv = "ByteSpanTest";
        auto converted_span = to_byte_span(original_sv);

        // 验证转换后的内容
        std::cout << "Original string: " << original_sv << "\n";
        std::cout << "Converted span content: ";
        for (auto b : converted_span)
        {
            std::cout << static_cast<char>(b);
        }
        std::cout << "\n";
        std::cout << "Bytes equal: " << equal_value(converted_span, original_sv)
                  << "\n\n";

        // 示例 3: 互转后内容一致性
        const std::string test_str = "ConversionTest";
        auto test_span = to_byte_span(test_str);
        auto test_sv = to_string_view(test_span);

        std::cout << "Roundtrip consistency: " << (test_sv == test_str) << "\n";
        std::cout << "Memory match: "
                  << (test_span.data() ==
                      reinterpret_cast<const uint8_t *>(test_str.data()))
                  << "\n";
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND