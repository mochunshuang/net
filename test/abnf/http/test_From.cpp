#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // From = mailbox
    constexpr auto from_pass = make_pass_test<mcs::abnf::http::From>();
    constexpr auto from_fail = make_unpass_test<mcs::abnf::http::From>();
    static_assert(from_pass("user@example.com"_span));   // 标准邮箱
    static_assert(from_pass("admin <admin@site>"_span)); // 带显示名
    static_assert(from_fail("invalid@host@test"_span));  // 多重@符号

    // 测试带特殊符号的邮箱
    static_assert(from_pass("\"user.name\"@domain.com"_span)); // 引号包裹本地部分

    // An example is:
    //  From: spider-admin@example.org
    static_assert(from_pass("spider-admin@example.org"_span));
    return 0;
}
// NOLINTEND