#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Vary = [ ( "*" / field-name ) *( OWS "," OWS ( "*" / field-name ) ) ]
    constexpr auto vary_pass = make_pass_test<mcs::abnf::http::Vary>();
    constexpr auto vary_fail = make_unpass_test<mcs::abnf::http::Vary>();

    // 有效Vary测试
    static_assert(vary_pass("*"_span));                           // 单独星号
    static_assert(vary_pass("User-Agent, Accept-Language"_span)); // 字段列表
    static_assert(vary_pass("Accept-Encoding"_span));             // 单个字段
    static_assert(vary_pass("*  ,  Content-Type"_span));          // 混合类型

    // 无效Vary测试
    static_assert(vary_fail("*,invalid field"_span)); // 非法字段名
    static_assert(vary_fail("User Agent"_span));      // 含空格字段
    static_assert(vary_fail("**, Accept"_span));      // 重复星号
    static_assert(vary_fail(",,,"_span));             // 空列表项

    // NOTE: 空格不能随便
    static_assert(vary_fail("  *  ,  Content-Type  "_span)); // 混合类型

    // 示例：内容协商资源上的实体标签变化
    /*
>> Request:

GET /index HTTP/1.1
Host: www.example.com
Accept-Encoding: gzip

在这种情况下，响应可能使用也可能不使用gzip内容编码。如果没有，响应可能看起来像
>> Response:

HTTP/1.1 200 OK
Date: Fri, 26 Mar 2010 00:05:00 GMT
ETag: "123-a"
Content-Length: 70
Vary: Accept-Encoding
Content-Type: text/plain

Hello World!
Hello World!
Hello World!
Hello World!
Hello World!

使用gzip内容编码的另一种表示是：
>> Response:

HTTP/1.1 200 OK
Date: Fri, 26 Mar 2010 00:05:00 GMT
ETag: "123-b"
Content-Length: 43
Vary: Accept-Encoding
Content-Type: text/plain
Content-Encoding: gzip

...binary data...

注意：内容编码是表示数据的属性，因此内容编码表示的强实体标记必须与未编码表示的实体标记不同，以防止缓存更新和范围请求期间的潜在冲突。相反，传输编码（[HTTP/1.1]第7节）仅在消息传输期间适用，不会导致不同的实体标记。
    */
    return 0;
}
// NOLINTEND