#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto tchar = [](OCTET v) constexpr {
        auto rule = mcs::abnf::http::tchar{};
        OCTET arr[1] = {v};
        auto ctx = make_parser_ctx(arr);
        rule(ctx);
        return ctx.done();
    };
    // 明确测试每个合法符号
    static_assert(tchar('!'));
    static_assert(tchar('#'));
    static_assert(tchar('$'));
    static_assert(tchar('%'));
    static_assert(tchar('&'));
    static_assert(tchar('\''));
    static_assert(tchar('*'));
    static_assert(tchar('+'));
    static_assert(tchar('-'));
    static_assert(tchar('.'));
    static_assert(tchar('^'));
    static_assert(tchar('_'));
    static_assert(tchar('`'));
    static_assert(tchar('|'));
    static_assert(tchar('~'));

    // 测试边界值
    static_assert(tchar('0')); // DIGIT 下限
    static_assert(tchar('9')); // DIGIT 上限
    static_assert(tchar('A')); // ALPHA 大写下限
    static_assert(tchar('Z')); // ALPHA 大写上限
    static_assert(tchar('a')); // ALPHA 小写下限
    static_assert(tchar('z')); // ALPHA 小写上限

    // 测试非法字符（随机选取典型值）
    static_assert(!tchar(' '));    // 空格
    static_assert(!tchar('"'));    // DQUOTE
    static_assert(!tchar('('));    // 未在 tchar 列表中
    static_assert(!tchar('\\'));   // 反斜杠
    static_assert(!tchar('@'));    // 商业符号
    static_assert(!tchar('['));    // 方括号
    static_assert(!tchar('\0'));   // 空字符
    static_assert(!tchar('\x7F')); // DEL 字符

    return 0;
}
// NOLINTEND