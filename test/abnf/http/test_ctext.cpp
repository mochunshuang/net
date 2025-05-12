#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // ctext = HTAB / SP / %x21-27 / %x2A-5B / %x5D-7E / obs-text
    constexpr auto ctext = [](OCTET a) constexpr {
        auto rule = mcs::abnf::http::ctext{};
        OCTET arr[] = {a};
        auto ctx = make_parser_ctx(arr);
        rule(ctx);
        return ctx.done();
    };

    // 有效ctext测试
    static_assert(ctext('\t'));   // 水平制表符
    static_assert(ctext('!'));    // 边界字符
    static_assert(ctext('\x7E')); // 最大允许字符
    static_assert(ctext('\x80')); // 扩展ASCII

    // 无效ctext测试
    static_assert(not ctext(')'));    // 禁止的字符
    static_assert(not ctext('\x00')); // 空字符
    static_assert(not ctext('\\'));   // 反斜杠（需转义）
    return 0;
}
// NOLINTEND