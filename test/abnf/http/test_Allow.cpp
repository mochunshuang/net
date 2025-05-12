#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Allow = [ method *( OWS "," OWS method ) ]
    constexpr auto allow_pass = make_pass_test<mcs::abnf::http::Allow>();
    constexpr auto allow_fail = make_unpass_test<mcs::abnf::http::Allow>();
    static_assert(allow_pass("GET, HEAD, PUT"_span)); // 标准方法
    static_assert(allow_pass("CUSTOM-METHOD"_span));  // 自定义方法
    static_assert(allow_fail("GET POST"_span));       // 缺少逗号
    static_assert(allow_fail("DELETE, "_span));       // 空尾项

    // 测试标准方法大小写敏感性（RFC 7230§3.1.1）
    static_assert(allow_pass("get"_span)); // 应视为合规（实际需根据解析器是否大小写敏感）
    static_assert(allow_fail("CUSTOM@METHOD"_span)); // 非法字符
    return 0;
}
// NOLINTEND