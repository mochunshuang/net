

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf::uri; // NOLINT
using OCTET = mcs::abnf::OCTET;

// 辅助函数用于简化测试断言
template <size_t N>
constexpr bool test_path_valid(const OCTET (&arr)[N])
{
    return path(std::span<const OCTET>{arr});
}
template <size_t N>
constexpr bool test_path_invalid(const OCTET (&arr)[N])
{
    return !path(std::span<const OCTET>{arr});
}

int main()
{

    // 1. path-abempty 测试 (以/开头或为空)
    { // 空路径

        static_assert(path(mcs::abnf::empty_span));
    }

    { // 仅斜杠
        static constexpr OCTET slash_only[] = {'/'};
        static_assert(test_path_valid(slash_only));
    }

    { // 多斜杠
        static constexpr OCTET multiple_slashes[] = {'/', '/', '/'};
        static_assert(test_path_valid(multiple_slashes));
    }

    { // 标准路径
        static constexpr OCTET standard_path[] = {'/', 'a', '/', 'b', '/', 'c'};
        static_assert(test_path_valid(standard_path));
    }

    // 2. path-absolute 测试 (以/开头但不以//开头)
    { // 单segment
        static constexpr OCTET absolute1[] = {'/', 'a'};
        static_assert(test_path_valid(absolute1));
    }

    { // 多segment
        static constexpr OCTET absolute2[] = {'/', 'a', '/', 'b', '/', 'c'};
        static_assert(test_path_valid(absolute2));
    }

    { // 包含编码
        static constexpr OCTET absolute_encoded[] = {'/', '%', '4', '1', '/', 'b'};
        static_assert(test_path_valid(absolute_encoded));
    }

    // 3. path-noscheme 测试 (以非冒号segment开头)
    { // 简单segment
        static constexpr OCTET noscheme1[] = {'a', 'b', 'c'};
        static_assert(test_path_valid(noscheme1));
    }

    { // 带特殊字符
        static constexpr OCTET noscheme2[] = {'a', '@', 'b', '/', 'c'};
        static_assert(test_path_valid(noscheme2));
    }

    { // 包含编码
        static constexpr OCTET noscheme_encoded[] = {'a', '%', '4', '1', '/', 'b'};
        static_assert(test_path_valid(noscheme_encoded));
    }

    // 4. path-rootless 测试 (以segment开头)
    { // 简单segment
        static constexpr OCTET rootless1[] = {'a', ':', 'b'};
        static_assert(test_path_valid(rootless1));
    }

    { // 多segment
        static constexpr OCTET rootless2[] = {'a', ':', 'b', '/', 'c'};
        static_assert(test_path_valid(rootless2));
    }

    { // 包含编码
        static constexpr OCTET rootless_encoded[] = {'a', ':', '%', '4', '1', '/', 'b'};
        static_assert(test_path_valid(rootless_encoded));
    }

    // 5. path-empty 测试
    { // 空路径

        static_assert(path(mcs::abnf::empty_span));
    }

    // 无效路径测试
    { // 包含空格
        static constexpr OCTET invalid_space[] = {' ', 'a', '/', 'b'};
        static_assert(test_path_invalid(invalid_space));
    }

    { // 包含非法字符
        static constexpr OCTET invalid_char[] = {'/', 'a', '#', 'b'};
        static_assert(test_path_invalid(invalid_char));
    }

    { // 不完整编码
        static constexpr OCTET incomplete_encoding[] = {'/', 'a', '%', '2'};
        static_assert(test_path_invalid(incomplete_encoding));
    }

    {
        static constexpr OCTET invalid_absolute[] = {'/', '/', 'a'};
        static_assert(path(invalid_absolute));
    }
    // 正确的path-absolute测试
    { // 有效绝对路径
        static constexpr OCTET valid_absolute1[] = {'/', 'a'};
        static constexpr OCTET valid_absolute2[] = {'/', 'a', '/', 'b'};
        static_assert(test_path_valid(valid_absolute1));
        static_assert(test_path_valid(valid_absolute2));
    }

    { // 双斜杠
        static constexpr OCTET single_slash[] = {'/', '/'};
        static_assert(not path_absolute(single_slash)); // path-absolute不允许
        static_assert(path_abempty(single_slash));      // 但path-abempty允许
    }

    // 正确的path-abempty测试
    { // 多斜杠路径
        static constexpr OCTET multi_slash[] = {'/', '/', 'a', '/', '/', 'b'};
        static_assert(test_path_valid(multi_slash)); // 有效的path-abempty
    }

    { // 包含冒号的非法路径
        static constexpr OCTET invalid_colon[] = {':', 'a', '/', 'b'};
        static_assert(path(invalid_colon));
    }

    // 边界情况测试
    { // 最大长度segment
        static constexpr OCTET max_segment[] = {
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
            'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a'};
        static_assert(test_path_valid(max_segment));
    }

    { // 复杂混合路径
        static constexpr OCTET complex_path[] = {
            '/', 'a', 'b', '/', '%', '4', '1', '/', 'c', 'd', ':', 'e', '/', 'f',
            '@', 'g', '/', 'h', 'i', 'j', '/', 'k', 'l', 'm', '/', 'n', 'o', 'p',
            '/', 'q', 'r', 's', '/', 't', 'u', 'v', '/', 'w', 'x', 'y', 'z'};
        static_assert(test_path_valid(complex_path));
    }

    return 0;
}
// NOLINTEND