#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

auto make_long_valid_token68()
{
    constexpr std::string_view valid_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
        "-._~+/";

    std::array<OCTET, 1024> arr{};
    // 填充合法字符
    for (size_t i = 0; i < arr.size() - 3; ++i)
    {
        arr[i] = static_cast<OCTET>(valid_chars[i % valid_chars.size()]);
    }
    // 末尾添加等号
    arr[arr.size() - 3] = '=';
    arr[arr.size() - 2] = '=';
    arr[arr.size() - 1] = '=';
    return arr;
}

int main()
{
    // 合法测试用例
    {
        // 基础合法字符
        static_assert(token68("a"_span));
        static_assert(token68("Z5-._~+/"_span));
        static_assert(token68("key=="_span));
        static_assert(token68("data=="_span));

        // 等号位置测试
        static_assert(token68("withequals====="_span));
        static_assert(not token68("with=equals====="_span)); // 等号之后必须全是等号
        static_assert(token68("no_equals"_span));            // 无等号
        static_assert(token68("only====="_span)); // 纯等号（非法，需至少一个非等号字符）

        // 混合字符
        static_assert(token68(
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~+/==="_span));

        // 极长输入（1024字符）
        auto long_token = make_long_valid_token68();

        assert(token68(long_token));
    }
    // 非法测试用例
    {
        // 空输入
        static_assert(!token68(""_span));

        // 非法字符
        static_assert(!token68("space here"_span));  // 空格
        static_assert(!token68("quote\""_span));     // DQUOTE
        static_assert(!token68("bad@char"_span));    // @
        static_assert(!token68("control\x01"_span)); // 控制字符

        // 等号不在末尾
        static_assert(!token68("in=valid=here"_span)); // 中间等号
        static_assert(!token68("=start"_span));        // 等号开头

        // 仅等号（无有效字符）
        static_assert(!token68("=="_span)); // 纯等号
    }
    // 边界测试
    {
        // 单字符 + 等号
        static_assert(token68("a="_span));

        // 最大允许等号数量（根据具体规则，假设无限制）
        static_assert(token68("key======================================"_span));

        // 混合末尾等号与非法字符
        static_assert(!token68("valid=invalid\x7F"_span)); // 末尾非法字符
    }

    return 0;
}
// NOLINTEND