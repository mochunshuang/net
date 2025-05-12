#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Connection = [ connection-option *( OWS "," OWS connection-option ) ]
    constexpr auto connection_pass = make_pass_test<mcs::abnf::http::Connection>();
    constexpr auto connection_fail = make_unpass_test<mcs::abnf::http::Connection>();
    static_assert(connection_pass("keep-alive"_span));
    static_assert(connection_pass("Upgrade,  Close"_span)); // 多选项带OWS

    static_assert(connection_fail("keep-alive; "_span));     // 非法字符
    static_assert(connection_fail("Upgrade,  Close "_span)); // 尾OWS

    return 0;
}
// NOLINTEND