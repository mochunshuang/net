#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // connection-option = token
    constexpr auto connection_option_pass =
        make_pass_test<mcs::abnf::http::connection_option>();
    constexpr auto connection_option_fail =
        make_unpass_test<mcs::abnf::http::connection_option>();

    // 有效connection-option测试
    static_assert(connection_option_pass("Keep-Alive"_span));
    static_assert(connection_option_pass("Upgrade"_span));

    // 无效connection-option测试
    static_assert(connection_option_fail("Close,"_span));
    static_assert(connection_option_fail("invalid option"_span));

    return 0;
}
// NOLINTEND