#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto qdtext = [](octet a) constexpr {
        auto rule = mcs::abnf::http::qdtext{};
        octet arr[] = {a};
        auto ctx = make_parser_ctx(arr);
        rule(ctx);
        return ctx.done();
    };
    // 明确测试边界值
    static_assert(qdtext(0x09)); // HTAB
    static_assert(qdtext(0x20)); // SP
    static_assert(qdtext('!'));  // 0x21
    static_assert(qdtext('#'));  // 0x23（下限）
    static_assert(qdtext('['));  // 0x5B（上限）
    static_assert(qdtext(']'));  // 0x5D（下限）
    static_assert(qdtext('~'));  // 0x7E（上限）
    static_assert(qdtext(0x80)); // obs-text 下限
    static_assert(qdtext(0xFF)); // obs-text 上限

    // 测试非法字符
    static_assert(!qdtext('"'));  // 0x22 (DQUOTE)
    static_assert(!qdtext('\\')); // 0x5C
    static_assert(!qdtext('\0')); // 0x00
    static_assert(!qdtext(0x7F)); // DEL
    static_assert(!qdtext(0x1B)); // ESC

    return 0;
}
// NOLINTEND