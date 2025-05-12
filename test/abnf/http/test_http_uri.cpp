#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // http-URI = "http://" authority path-abempty [ "?" query ]
    // authority     = [ userinfo "@" ] host [ ":" port ]
    //  host          = IP-literal / IPv4address / reg-name
    //  reg-name      = *( unreserved / pct-encoded / sub-delims )
    // path-abempty  = *( "/" segment )
    // 测试HTTP URI的有效和无效情况
    constexpr auto http_URI = make_pass_test<mcs::abnf::http::http_URI>();
    constexpr auto not_http_URI = make_unpass_test<mcs::abnf::http::http_URI>();

    {
        // 有效HTTP URI测试用例
        static_assert(http_URI("http://example.com"_span));
        static_assert(http_URI("http://example.com/"_span));
        static_assert(http_URI("http://user@example.com:80"_span));
        static_assert(http_URI("http://192.168.1.1/path?query=123"_span));
        static_assert(http_URI("http://[2001:db8::1]/api?v=1"_span));

        // 无效HTTP URI测试用例
        static_assert(not not_http_URI("https://example.com"_span)); // 协议头错误

        static_assert(http_URI("http://"_span));      // 缺少authority
        static_assert(http_URI("http:///path"_span)); // 空authority

        static_assert(
            not_http_URI("http://example.com#fragment"_span)); // 包含非法fragment

        // NOTE: ABNF默认不区分大小写
        static_assert(http_URI("HTTP://example.com"_span)); // 协议头大小写错误
    }

    // 测试HTTPS URI的有效和无效情况
    constexpr auto https_URI = make_pass_test<mcs::abnf::http::https_URI>();
    constexpr auto not_https_URI = make_unpass_test<mcs::abnf::http::https_URI>();

    // 有效HTTPS URI测试用例
    static_assert(https_URI("https://example.com"_span));
    static_assert(https_URI("https://user:pass@example.com:443/path?_q=1"_span));
    static_assert(https_URI("https://example.com/path/to%20file"_span));
    static_assert(https_URI("https://[::1]/?query"_span));

    // 无效HTTPS URI测试用例
    static_assert(not not_https_URI("http://example.com"_span));   // 协议头错误
    static_assert(not_https_URI("https://example.com:port"_span)); // 非法端口号
    static_assert(
        not_https_URI("https://example.com/path#frag"_span)); // 包含非法fragment

    static_assert(https_URI("https:///dummy"_span)); // 空authority

    return 0;
}
// NOLINTEND