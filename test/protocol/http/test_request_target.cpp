#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    /**
         * request-target = origin-form / absolute-form / authority-form /
                            asterisk-form
         */
    // ==================== request-target 规则测试 ====================
    constexpr auto request_target_pass = make_pass_test<request_target>();
    constexpr auto request_target_fail = make_unpass_test<request_target>();

    // 1. origin-form 测试
    static_assert(request_target_pass("/path/to/resource"_span)); // 基础路径
    static_assert(request_target_pass("/"_span));                 // 根路径
    static_assert(request_target_pass("/search?q=term"_span));    // 带查询参数
    static_assert(request_target_pass("/data;type=json"_span));   // 路径参数

    // NOTE: 不包括 #frag
    //  static_assert(request_target_pass("/a/b/c?x=1&y=2#frag"_span)); // 完整origin-form
    static_assert(request_target_pass("/a/b/c?x=1&y=2"_span));

    // 2. absolute-form 测试
    static_assert(request_target_pass("https://example.com/index.html"_span)); // HTTP URL
    static_assert(
        request_target_pass("ftp://files.example.com/pub/file.txt"_span)); // FTP URL
    static_assert(request_target_pass(
        "https://user:pass@example.com:8080/path?q=term"_span)); // 完整URL

    static_assert(request_target_pass(":80"_span));          // 缺少主机
    static_assert(request_target_pass("example.com:"_span)); // 缺少端口号

    // 3. authority-form 测试
    static_assert(request_target_pass("example.com:80"_span));    // 主机+端口
    static_assert(request_target_pass("localhost:8080"_span));    // 本地主机
    static_assert(request_target_pass("[2001:db8::1]:443"_span)); // IPv6地址
    static_assert(request_target_pass("192.168.1.1:8443"_span));  // IPv4地址

    // 4. asterisk-form 测试
    static_assert(request_target_pass("*"_span)); // 仅星号

    // 边界情况测试
    static_assert(request_target_pass("/"_span));             // 最小origin-form
    static_assert(request_target_pass("*"_span));             // 最小asterisk-form
    static_assert(request_target_pass("example.com:0"_span)); // 最小端口
    static_assert(request_target_pass("a:b"_span));           // 最小authority-form

    // ==================== 无效 request-target 测试 ====================
    static_assert(!request_target_pass(""_span)); // 空目标
    static_assert(
        !request_target_fail("example.com"_span)); // 缺少端口(无效authority-form)
    static_assert(!request_target_fail("path/without/root"_span)); // 缺少前导斜杠
    static_assert(
        request_target_fail("https://example.com/path space"_span)); // URL包含空格
    static_assert(
        request_target_fail("https://example.com/\tpath"_span)); // URL包含制表符
    static_assert(request_target_fail("*extra"_span));           // 星号后有额外字符
    static_assert(request_target_fail("/path\n"_span));          // 包含控制字符

    static_assert(!request_target_fail("[invalid:ip]::443"_span)); // 无效IPv6格式

    // ==================== 特殊字符测试 ====================
    static_assert(request_target_pass("/%20encoded%21"_span));  // 百分比编码
    static_assert(request_target_pass("/~user/file.txt"_span)); // 允许的符号
    static_assert(request_target_pass("/a-b_c/d.e"_span));      // 允许的标点
    static_assert(request_target_fail("/path\u0001"_span));     // 控制字符(U+0001)
    static_assert(
        request_target_fail("https://example.com/\u007F"_span)); // DEL字符(U+007F)

    // ==================== 各子规则详细测试 ====================

    // origin-form 详细测试
    constexpr auto origin_form_pass = make_pass_test<origin_form>();
    constexpr auto origin_form_fail = make_unpass_test<origin_form>();

    static_assert(origin_form_pass("/a/b/c"_span));
    static_assert(origin_form_pass("/?query"_span));
    static_assert(origin_form_pass("/path?x=1&y=2"_span));
    static_assert(origin_form_pass("//double/slash"_span));

    static_assert(!origin_form_fail("path/without/slash"_span));

    static_assert(!origin_form_fail("http://example.com"_span)); // 应使用absolute-form

    // absolute-form 详细测试
    // absolute-URI  = scheme ":" hier-part [ "?" query ]
    // scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    constexpr auto absolute_form_pass = make_pass_test<absolute_form>();
    constexpr auto absolute_form_fail = make_unpass_test<absolute_form>();

    static_assert(absolute_form_pass("https://example.com"_span));
    static_assert(absolute_form_pass("http://localhost:8080/path"_span));
    static_assert(!absolute_form_fail("/relative/path"_span));

    static_assert(!make_rule_test<origin_form>("example.com:80"_span)
                       .first); // 应使用authority-form
    {
        static_assert(make_pass_test<authority_form>()("example.com:80"_span));
    }
    static_assert(
        !make_rule_test<origin_form>("mailto:user@example.com"_span).first); // 非HTTP URI

    // authority-form 详细测试
    constexpr auto authority_form_pass = make_pass_test<authority_form>();
    constexpr auto authority_form_fail = make_unpass_test<authority_form>();

    static_assert(authority_form_pass("example.com:443"_span));
    static_assert(authority_form_pass("[::1]:8080"_span));
    static_assert(authority_form_pass("192.0.2.1:80"_span));

    static_assert(authority_form_pass(":80"_span));          // 缺少主机
    static_assert(authority_form_pass("host:99999"_span));   // 端口过大
    static_assert(!authority_form_fail("example.com"_span)); // 缺少端口
    static_assert(authority_form_fail("host:port"_span));    // 非数字端口

    // asterisk-form 详细测试
    constexpr auto asterisk_form_pass = make_pass_test<asterisk_form>();
    constexpr auto asterisk_form_fail = make_unpass_test<asterisk_form>();

    static_assert(asterisk_form_pass("*"_span));

    static_assert(!asterisk_form_fail(" * "_span)); // 不允许空格
    static_assert(asterisk_form_fail("**"_span));   // 多个星号
    static_assert(asterisk_form_fail("*/"_span));   // 额外字符
    return 0;
}
// NOLINTEND