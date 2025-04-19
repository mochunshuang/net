#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 仅有 pseudonym
        constexpr auto s1 = "server01"_span;
        static_assert(received_by(s1));

        // 含合法端口（边界值）
        constexpr auto s2 = "example.com:0"_span; // 最小端口
        constexpr auto s3 = "api:65535"_span;     // 最大端口
        constexpr auto s4 = "host_123:8080"_span; // 常规端口
        static_assert(received_by(s2));
        static_assert(received_by(s3));
        static_assert(received_by(s4));

        // 特殊字符 pseudonym（根据 token 规则）
        constexpr auto s5 = "custom~host+name:443"_span;
        static_assert(received_by(s5));

        // 极长 pseudonym 和端口
        constexpr auto s6 = "abcdefghijklmnopqrstuvwxyz0123456789-._~:12345"_span;
        static_assert(received_by(s6));
    }
    // 非法测试用例
    {
        // pseudonym 含非法字符
        constexpr auto s1 = "host name:80"_span;   // 空格
        constexpr auto s2 = "user@host:8080"_span; // @符号
        static_assert(not received_by(s1));
        static_assert(not received_by(s2));

        // 端口非法值
        constexpr auto s3 = "server:65536"_span;     // 超最大端口
        constexpr auto s4 = "localhost:123abc"_span; // 非数字端口
        constexpr auto s5 = "host:-80"_span;         // 负号
        static_assert(received_by(s3));
        static_assert(not received_by(s4));
        static_assert(not received_by(s5));

        // 结构错误
        constexpr auto s6 = ":8080"_span;    // 空 pseudonym
        constexpr auto s7 = "host:"_span;    // 空端口
        constexpr auto s8 = "host::80"_span; // 多个冒号
        static_assert(not received_by(s6));
        static_assert(received_by(s7));
        static_assert(not received_by(s8));

        // 空输入
        constexpr auto s9 = ""_span;
        static_assert(not received_by(s9));
    }

    return 0;
}
// NOLINTEND