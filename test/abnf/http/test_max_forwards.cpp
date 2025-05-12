#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // Max-Forwards = 1*DIGIT
    constexpr auto max_forwards_pass = make_pass_test<mcs::abnf::http::Max_Forwards>();
    constexpr auto max_forwards_fail = make_unpass_test<mcs::abnf::http::Max_Forwards>();
    static_assert(max_forwards_pass("0"_span));          // 边界值
    static_assert(max_forwards_pass("2147483647"_span)); // 极大值

    static_assert(max_forwards_pass("0123"_span)); // 前导零（根据规范允许但可自定义限制）

    static_assert(!max_forwards_fail("-5"_span)); // 负值

    return 0;
}
// NOLINTEND