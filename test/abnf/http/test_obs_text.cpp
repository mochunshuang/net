#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto obs_text = [](OCTET a) {
        auto rule = mcs::abnf::http::obs_text{};
        OCTET arr[] = {a};
        auto ctx = make_parser_ctx(arr);
        rule(ctx);
        return ctx.done();
    };
    // 明确测试边界值
    static_assert(obs_text(0x80)); // obs-text 下限
    static_assert(obs_text(0xFF)); // obs-text 上限

    return 0;
}
// NOLINTEND