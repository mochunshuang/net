#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // etagc = "!" / %x23-7E / obs-text
    constexpr auto etagc = [](OCTET a) constexpr {
        auto rule = mcs::abnf::http::etagc{};
        OCTET arr[] = {a};
        auto ctx = make_parser_ctx(arr);
        rule(ctx);
        return ctx.done();
    };

    // 有效 etagc 测试
    static_assert(etagc('!'));    // 单独感叹号
    static_assert(etagc('#'));    // 有效起始字符
    static_assert(etagc('~'));    // 有效终止字符
    static_assert(etagc('\x80')); // obs-text (扩展ASCII)

    // 无效 etagc 测试
    static_assert(not etagc('\"'));   // 不允许双引号
    static_assert(not etagc(' '));    // 空格
    static_assert(not etagc('\x00')); // 控制字符
    static_assert(not etagc('\x7F')); // DEL字符
    return 0;
}
// NOLINTEND