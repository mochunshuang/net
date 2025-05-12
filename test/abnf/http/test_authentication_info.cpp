
#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // Authentication-Info = [ auth-param *( OWS "," OWS auth-param ) ]
    constexpr auto auth_info_pass =
        make_pass_test<mcs::abnf::http::Authentication_Info>();
    constexpr auto auth_info_fail =
        make_unpass_test<mcs::abnf::http::Authentication_Info>();
    static_assert(auth_info_pass("nextnonce=\"abc\""_span));       // 单个参数
    static_assert(auth_info_pass("rspauth=foo, cnonce=bar"_span)); // 多参数
    static_assert(auth_info_fail("key=,value"_span));              // 空键值
    static_assert(auth_info_fail("param=unclosed\""_span));        // 引号未闭合

    // 测试复杂参数值
    static_assert(auth_info_pass("digest=md5,stale=true"_span));
    static_assert(auth_info_fail("key==value"_span)); // 双重等号
    return 0;
}
// NOLINTEND