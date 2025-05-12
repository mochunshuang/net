#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Location = URI-reference
    constexpr auto location_pass = make_pass_test<mcs::abnf::http::Location>();
    // constexpr auto location_fail = make_pass_test<mcs::abnf::http::Location>();
    static_assert(location_pass("https://example.com/new"_span)); // 绝对URI
    static_assert(location_pass("/relative/path"_span));          // 相对路径
    static_assert(location_pass("javascript:alert(1)"_span));     // 危险协议

    static_assert(location_pass("http://[2001:db8::1]/path"_span)); // IPv6地址

    return 0;
}
// NOLINTEND