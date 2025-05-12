#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // challenge = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS "," OWS auth-param )
    // ] ) ]
    // credentials = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS "," OWS auth-param
    // ) ] ) ]
    // 测试用例与challenge结构相同，根据实际规范调整参数
    constexpr auto credentials_pass = make_pass_test<mcs::abnf::http::credentials>();
    constexpr auto credentials_fail = make_unpass_test<mcs::abnf::http::credentials>();
    static_assert(credentials_pass("Basic dXNlcjpwYXNz"_span)); // Base64凭证
    static_assert(credentials_fail("Bearer token=1,2"_span));   // 非法参数格式

    return 0;
}
// NOLINTEND