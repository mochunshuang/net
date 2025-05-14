#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto method = make_pass_test<mcs::abnf::http::method>();
    static_assert(method("GET"_span) && method("HEAD"_span) && method("POST"_span) &&
                  method("PUT"_span) && method("DELETE"_span) && method("CONNECT"_span) &&
                  method("OPTIONS"_span) && method("TRACE"_span));
    /*
请求方法令牌是请求语义学的主要来源；它指示客户端发出此请求的目的以及客户端期望的成功结果。
GET     传输目标资源的当前表示。
HEAD    与GET相同，但不传输响应内容。
POST    对请求内容执行特定于资源的处理。
PUT     将目标资源的所有当前表示替换为请求内容。
DELETE  删除目标资源的所有当前表示。
CONNECT 建立到目标资源标识的服务器的隧道。
OPTIONS 描述目标资源的通信选项。
TRACE   沿着目标资源的路径执行消息环回测试。
*@see https://www.rfc-editor.org/rfc/rfc9110.html#table-4
    */
    return 0;
}
// NOLINTEND