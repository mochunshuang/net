#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Content-Location = absolute-URI / partial-URI
    constexpr auto content_loc_pass = make_pass_test<mcs::abnf::http::Content_Location>();
    // constexpr auto content_loc_fail =
    // make_pass_test<mcs::abnf::http::Content_Location>();
    static_assert(content_loc_pass("https://example.com/doc"_span)); // 绝对URI
    static_assert(content_loc_pass("/path/to/resource"_span));       // 相对URI
    static_assert(content_loc_pass("ftp://example.com"_span));       // 禁用协议

    return 0;
}
// NOLINTEND