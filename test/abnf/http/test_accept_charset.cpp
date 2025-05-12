#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Accept-Charset = [ ( ( token / "*" ) [ weight ] ) *( OWS "," OWS ( token / "*" ) [
    // weight ] ) ]
    constexpr auto accept_charset_pass =
        make_pass_test<mcs::abnf::http::Accept_Charset>();
    constexpr auto accept_charset_fail =
        make_unpass_test<mcs::abnf::http::Accept_Charset>();
    static_assert(accept_charset_pass("utf-8, iso-8859-1;q=0.5"_span)); // 有效字符集
    static_assert(accept_charset_pass("*"_span));                       // 仅通配符
    static_assert(accept_charset_pass("*, utf-8"_span));                // 混合通配符

    static_assert(accept_charset_fail("utf-8;q=2"_span));    // 非法权重
    static_assert(accept_charset_fail("application/"_span)); // 不完整类型

    return 0;
}
// NOLINTEND