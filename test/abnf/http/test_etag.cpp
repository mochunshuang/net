#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // ETag = entity-tag
    constexpr auto etag_pass = make_pass_test<mcs::abnf::http::ETag>();
    constexpr auto etag_fail = make_unpass_test<mcs::abnf::http::ETag>();
    static_assert(etag_pass("\"abc123\""_span)); // 强ETag
    static_assert(etag_pass("W/\"xyz\""_span));  // 弱ETag

    static_assert(!etag_fail("unquoted"_span)); // 未加引号
    return 0;
}
// NOLINTEND