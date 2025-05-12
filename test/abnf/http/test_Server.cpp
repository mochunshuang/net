#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // Server = product *( RWS ( product / comment ) )
    constexpr auto server_pass = make_pass_test<mcs::abnf::http::Server>();
    constexpr auto server_fail = make_unpass_test<mcs::abnf::http::Server>();

    static_assert(server_pass("Apache/2.4.1 (Unix)"_span));      // 标准格式
    static_assert(server_pass("nginx\t1.25.3\t(SSL/TLS)"_span)); // 水平制表符
    static_assert(server_fail("Server/1.0(no space)"_span));     // 缺少RWS
    static_assert(server_fail("Invalid/Server@1"_span));         // 非法字符

    return 0;
}
// NOLINTEND