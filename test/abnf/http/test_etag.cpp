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
    /**
     * 【响应】中的“ETag”字段为所选表示提供当前实体标签，在处理请求结束时确定
     * @brief Examples:
     *          ETag: "xyzzy"
     *          ETag: W/"xyzzy"
     *          ETag: ""
     * @see
     *https://www.rfc-editor.org/rfc/rfc9110.html#status.204:~:text=The-,example,-below%20shows%20the
     */
    static_assert(etag_pass("\"xyzzy\""_span));   // 强ETag
    static_assert(etag_pass("W/\"xyzzy\""_span)); // 弱ETag
    static_assert(etag_pass("\"\""_span));
    return 0;
}
// NOLINTEND