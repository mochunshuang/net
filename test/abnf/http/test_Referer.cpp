#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // Referer = absolute-URI / partial-URI
    constexpr auto referer_pass = make_pass_test<mcs::abnf::http::Referer>();
    constexpr auto referer_fail = make_unpass_test<mcs::abnf::http::Referer>();

    static_assert(referer_pass("https://example.com/path?q=1"_span)); // 绝对URI
    static_assert(referer_pass("/relative/path"_span));               // 部分URI
    static_assert(referer_pass("ftp://example.com"_span));            // 禁止的协议

    static_assert(referer_fail("http://[::1] extra"_span)); // 非法字符

    return 0;
}
// NOLINTEND