#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
    constexpr auto quoted_pair = [](octet a, octet b) constexpr {
        auto rule = mcs::abnf::http::quoted_pair{};
        octet arr[] = {a, b};
        auto ctx = make_parser_ctx(arr);
        rule(ctx);
        return ctx.done();
    };
    // 合法用例
    {

        // 反斜杠 + 特殊字符
        static_assert(quoted_pair('\\', mcs::abnf::htab_value)); // \t

        static_assert(quoted_pair('\\', mcs::abnf::sp_value)); // 空格
        static_assert(quoted_pair('\\', ' '));

        static_assert(quoted_pair('\\', '!')); // VCHAR 下限
        static_assert(quoted_pair('\\', '~')); // VCHAR 上限

        // 反斜杠 + obs-text
        static_assert(quoted_pair('\\', 0x80)); // obs-text 下限
        static_assert(quoted_pair('\\', 0xFF)); // obs-text 上限

        // 反斜杠 + 中间值
        static_assert(quoted_pair('\\', 'A'));
        static_assert(quoted_pair('\\', 'z'));
        static_assert(quoted_pair('\\', '5'));
        static_assert(quoted_pair('\\', '#'));

        // NOTE: \n or \r is good for quoted_pair
        static_assert(quoted_pair('\\', 'n')); // \n
        static_assert(quoted_pair('\\', 'r')); // \r
    }
    // 非法用例
    {
        // 第一个字符不是反斜杠
        static_assert(!quoted_pair('/', 't'));  // 错误引导符
        static_assert(!quoted_pair('a', 0x80)); // 非反斜杠

        // NOTE: " 开头也不行。可以区分于 quoted_string
        static_assert(!quoted_pair('"', 0x80)); // 非反斜杠
                                                //
        // 反斜杠 + 非法字符
        static_assert(!quoted_pair('\\', 0x00)); // NUL
        static_assert(!quoted_pair('\\', 0x1F)); // US

        static_assert(quoted_pair('\\', 0x7F)); // DEL

        // NOTE: 不能简单的 搜索 " 这是错误的。\" 是允许的
        //  验证 DQUOTE 的特殊性（VCHAR 包含 0x22）
        static_assert(quoted_pair('\\', '\"')); // 应通过（" 是 VCHAR）
    }

    // 边界测试
    {
        // VCHAR 边界
        static_assert(quoted_pair('\\', 0x21)); // '!' 合法
        static_assert(quoted_pair('\\', 0x7E)); // '~' 合法

        // obs-text 边界
        static_assert(quoted_pair('\\', 0x80));
        static_assert(quoted_pair('\\', 0xFF));
    }

    return 0;
}
// NOLINTEND