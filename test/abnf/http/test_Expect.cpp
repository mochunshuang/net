#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Expect = [ expectation *( OWS "," OWS expectation ) ]
    constexpr auto expect_pass = make_pass_test<mcs::abnf::http::Expect>();
    constexpr auto expect_fail = make_unpass_test<mcs::abnf::http::Expect>();
    static_assert(expect_pass("100-continue"_span));
    static_assert(expect_pass("custom=param, another"_span)); // 多期望

    static_assert(expect_fail("invalid=@value"_span)); // 非法参数

    // 本规范定义的唯一期望是“100-继续”（没有定义参数）。
    // https://www.rfc-editor.org/rfc/rfc9110.html#status.204:~:text=member%20other%20than-,100%2Dcontinue,-MAY%20respond%20with
    /*
For example, a request that begins with:

PUT /somewhere/fun HTTP/1.1
Host: origin.example.com
Content-Type: video/h264
Content-Length: 1234567890987
Expect: 100-continue

如果方法、目标URI和标头字段不足以导致立即成功、重定向或错误响应，“100-继续”期望通知接收者客户端即将在此请求中发送（可能很大）内容，并希望收到100（继续）临时响应。这允许客户端在实际发送内容之前等待值得发送内容的指示，这可以在数据很大或客户端预计可能会发生错误时提高效率（例如，第一次发送状态更改方法时，没有先前验证的身份验证凭据）。
    */
    return 0;
}
// NOLINTEND