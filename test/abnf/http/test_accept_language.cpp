#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Accept-Language = [ ( language-range [ weight ] ) *( OWS "," OWS ( language-range [
    // weight ] ) ]
    constexpr auto accept_lang_pass = make_pass_test<mcs::abnf::http::Accept_Language>();
    constexpr auto accept_lang_fail =
        make_unpass_test<mcs::abnf::http::Accept_Language>();
    static_assert(accept_lang_pass("zh-CN, en-US;q=0.7"_span)); // 带地区代码

    static_assert(accept_lang_fail("*-CH;q=0.5"_span)); // 通配地区
    static_assert(accept_lang_fail("en_gb"_span));      // 非法下划线
    static_assert(accept_lang_fail("zh;q=1.01"_span));  // 权重越界

    // 测试RFC 4647语言标签
    static_assert(accept_lang_pass("zh-Hans-CN"_span)); // 扩展子标签
    static_assert(accept_lang_pass("sr-Latn"_span));    // 脚本子标签
    static_assert(accept_lang_fail("zh_CN"_span));      // 错误分隔符
    return 0;
}
// NOLINTEND