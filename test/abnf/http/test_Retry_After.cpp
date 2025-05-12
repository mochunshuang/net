#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // Retry-After = HTTP-date / delay-seconds
    constexpr auto retry_after_pass = make_pass_test<mcs::abnf::http::Retry_After>();
    constexpr auto retry_after_fail = make_unpass_test<mcs::abnf::http::Retry_After>();

    static_assert(retry_after_pass("Wed, 21 Oct 2023 07:28:00 GMT"_span)); // 日期格式
    static_assert(retry_after_pass("3600"_span));                          // 数字格式
    static_assert(retry_after_fail("21 Oct 2023"_span));                   // 不完整日期
    static_assert(retry_after_fail("3.14"_span));                          // 非整数

    return 0;
}
// NOLINTEND