

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>

// NOLINTBEGIN

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= userinfo 有效用例 =================
    constexpr auto userinfo_1 = "john.doe_123-~"_span;   // 仅 unreserved 字符
    constexpr auto userinfo_2 = "!$&'()*+,;="_span;      // 所有 sub-delims 字符
    constexpr auto userinfo_3 = "user:pass"_span;        // 包含冒号
    constexpr auto userinfo_4 = "%41%7a%2A"_span;        // 合法 pct-encoded（A、z、*）
    constexpr auto userinfo_5 = "mix%ED:test!$'*+"_span; // 混合所有允许的类型

    static_assert(userinfo(userinfo_1));
    static_assert(userinfo(userinfo_2));
    static_assert(userinfo(userinfo_3));
    static_assert(userinfo(userinfo_4));
    static_assert(userinfo(userinfo_5));

    // 极端用例：包含所有允许字符的长组合
    constexpr auto userinfo_6 =
        "a~1-_.%21%24%26%27%28%29%2A%2B%2C%3B%3A%3D"_span; // 混合编码和字面符号
    static_assert(userinfo(userinfo_6));

    return 0;
}

// NOLINTEND