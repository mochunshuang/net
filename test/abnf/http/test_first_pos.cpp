#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // first-pos = 1*DIGIT
    constexpr auto first_pos = make_pass_test<mcs::abnf::http::first_pos>();
    constexpr auto not_first_pos = make_unpass_test<mcs::abnf::http::first_pos>();

    // 有效first-pos测试
    static_assert(first_pos("0"_span));
    static_assert(first_pos("12345"_span));
    static_assert(first_pos("9876543210"_span));
    static_assert(not first_pos(""_span)); // 空值

    // 无效first-pos测试
    static_assert(not_first_pos("12a3"_span));     // 含字母
    static_assert(not not_first_pos(" 123"_span)); // 前导空格
    static_assert(not not_first_pos("-123"_span)); // 负号

    return 0;
}
// NOLINTEND