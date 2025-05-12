#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // absolute-path = 1*( "/" segment )
    constexpr auto absolute_path_pass = make_pass_test<mcs::abnf::http::absolute_path>();
    constexpr auto absolute_path_fail =
        make_unpass_test<mcs::abnf::http::absolute_path>();

    // 有效absolute-path测试
    static_assert(absolute_path_pass("/"_span));                  // 根路径
    static_assert(absolute_path_pass("/api/v1"_span));            // 多级路径
    static_assert(absolute_path_pass("/path/with%20space"_span)); // URL编码
    static_assert(absolute_path_pass("//double//slashes"_span));  // 允许重复斜杠
    static_assert(not absolute_path_pass(""_span));               // 空路径
    static_assert(absolute_path_pass("/../parent"_span));         // 路径回溯

    // 无效absolute-path测试

    static_assert(not absolute_path_fail("relative/path"_span)); // 缺少起始斜杠
    static_assert(absolute_path_fail("/bad|path"_span));         // 非法字符

    return 0;
}
// NOLINTEND