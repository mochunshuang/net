

#include "../test_head.hpp"

#include <array>
#include <cassert>
#include <span>
#include <string_view>

// NOLINTBEGIN
using OCTET = mcs::abnf::OCTET;

#include <array>
#include <cstdint>
#include <span>

using OCTET = std::uint8_t;

#include <array>
#include <cstdint>
#include <span>

using OCTET = std::uint8_t;

constexpr auto test_span(mcs::abnf::span_param_in s)
{
    return s.size() > 0;
}

constexpr auto test_span_size(mcs::abnf::span_param_in s)
{
    return s.size();
}

int main()
{
    using namespace mcs::abnf::uri; // NOLINT
    constexpr auto v [[maybe_unused]] = make_array("123");
    // std::span<const OCTET> sp(v);
    [[maybe_unused]] constexpr auto v2 = test_span(make_array("123"));
    static_assert(test_span(make_array("123")));

    static_assert(test_span_size(make_array("123")) == 3);

    TEST("BASE") = [] {
        constexpr auto test_invalid_tail = make_array("v1.a!"); // tail 包含非法字符 '!'
        static_assert(test_invalid_tail[4] == '!');
        static_assert(test_invalid_tail.size() == 5);
        static_assert(unreserved('a'));
        static_assert(not unreserved(test_invalid_tail[4]));
        static_assert(sub_delims('!'));
    };

    // 有效测试用例
    constexpr auto valid_v1 = make_array("v1.x");           // 合法：'x' 是 unreserved
    constexpr auto valid_v2 = make_array("vA.:");           // 合法：':' 是允许字符
    constexpr auto valid_v3 = make_array("vF.~");           // 合法：'~' 是 unreserved
    constexpr auto valid_v4 = make_array("v0.!$&'()*+,;="); // 合法：全 sub-delims 字符
    constexpr auto valid_v5 = make_array("v123.a_b:z"); // 合法：混合 unreserved 和 ':'

    // 无效测试用例
    constexpr auto invalid_v1 = make_array("a1.x");    // 无效：不以 'v' 开头
    constexpr auto invalid_v2 = make_array("v.abc");   // 无效：'v' 后无 HEXDIG
    constexpr auto invalid_v3 = make_array("vG.abc");  // 无效：'G' 不是 HEXDIG
    constexpr auto invalid_v4 = make_array("v1a");     // 无效：缺少 '.' 和尾部
    constexpr auto invalid_v5 = make_array("v2.#");    // 无效：'#' 属于 gen-delims
    constexpr auto invalid_v6 = make_array("v3.");     // 无效：尾部为空
    constexpr auto invalid_v7 = make_array("v4a./?="); // 无效：包含 '/' 和 '?'

    // 静态断言验证
    // 有效用例应通过 IPvFuture 检查
    static_assert(IPvFuture(valid_v1));
    static_assert(IPvFuture(valid_v2));
    static_assert(IPvFuture(valid_v3));
    static_assert(IPvFuture(valid_v4));
    static_assert(IPvFuture(valid_v5));

    // 无效用例应不通过 IPvFuture 检查
    static_assert(!IPvFuture(invalid_v1));
    static_assert(!IPvFuture(invalid_v2));
    static_assert(!IPvFuture(invalid_v3));
    static_assert(!IPvFuture(invalid_v4));
    static_assert(!IPvFuture(invalid_v5));
    static_assert(!IPvFuture(invalid_v6));
    static_assert(!IPvFuture(invalid_v7));

    return 0;
}

// NOLINTEND