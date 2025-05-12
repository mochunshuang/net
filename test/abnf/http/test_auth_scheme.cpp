#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // auth-scheme = token
    // token = 1*tchar
    constexpr auto auth_scheme_pass = make_pass_test<mcs::abnf::http::auth_scheme>();
    constexpr auto auth_scheme_fail = make_unpass_test<mcs::abnf::http::auth_scheme>();

    // 有效scheme测试
    static_assert(auth_scheme_pass("Basic"_span));
    static_assert(auth_scheme_pass("Bearer_v2"_span));
    static_assert(auth_scheme_pass("OAuth-2.0"_span));
    static_assert(auth_scheme_pass("1.0"_span));  // 数字起始
    static_assert(not auth_scheme_pass(""_span)); // 空值

    // 无效scheme测试
    static_assert(auth_scheme_fail("Invalid Scheme"_span)); // 含空格
    static_assert(auth_scheme_fail("scheme@v1"_span));      // 非法字符

    return 0;
}
// NOLINTEND