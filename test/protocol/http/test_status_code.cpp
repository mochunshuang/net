#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    constexpr auto status_code_pass = make_pass_test<status_code>();
    constexpr auto status_code_fail = make_unpass_test<status_code>();

    static_assert(status_code_pass("200"_span));
    static_assert(status_code_pass("404"_span));
    static_assert(status_code_pass("999"_span)); // 虽然非标准但语法有效

    static_assert(!status_code_fail("20"_span));  // 不足三位
    static_assert(status_code_fail("2000"_span)); // 超过三位
    static_assert(!status_code_fail("2O0"_span)); // 包含字母
    static_assert(status_code_fail("200 "_span)); // 尾部空格

    return 0;
}
// NOLINTEND