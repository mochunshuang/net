#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Content-Length = 1*DIGIT
    constexpr auto content_len_pass = make_pass_test<mcs::abnf::http::Content_Length>();
    constexpr auto content_len_fail = make_unpass_test<mcs::abnf::http::Content_Length>();
    static_assert(content_len_pass("0"_span));                    // 边界值
    static_assert(content_len_pass("18446744073709551615"_span)); // 64位最大值
    static_assert(content_len_fail("12a3"_span));                 // 非法字符

    return 0;
}
// NOLINTEND