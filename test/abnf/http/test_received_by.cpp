#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        constexpr auto received_by_rule = make_pass_test<received_by>();
        // 仅有 pseudonym
        constexpr auto s1 = "server01"_span;
        static_assert(received_by_rule(s1));

        // 含合法端口（边界值）
        constexpr auto s2 = "example.com:0"_span; // 最小端口
        constexpr auto s3 = "api:65535"_span;     // 最大端口
        constexpr auto s4 = "host_123:8080"_span; // 常规端口
        static_assert(received_by_rule(s2));
        static_assert(received_by_rule(s3));
        static_assert(received_by_rule(s4));

        // 特殊字符 pseudonym（根据 token 规则）
        constexpr auto s5 = "custom~host+name:443"_span;
        static_assert(received_by_rule(s5));

        // 极长 pseudonym 和端口
        constexpr auto s6 = "abcdefghijklmnopqrstuvwxyz0123456789-._~:12345"_span;
        static_assert(received_by_rule(s6));
        {
            constexpr auto s3 = "server:65536"_span; // 超最大端口
            static_assert(received_by_rule(s3));

            constexpr auto s7 = "host:"_span; // 空端口
            static_assert(received_by_rule(s7));

            // 空输入
            constexpr auto s9 = ""_span;
            static_assert(not received_by_rule(s9));
        }
    }
    // 非法测试用例
    {
        constexpr auto received_by_rule = make_unpass_test<received_by>();
        // pseudonym 含非法字符
        constexpr auto s1 = "host name:80"_span;   // 空格
        constexpr auto s2 = "user@host:8080"_span; // @符号
        static_assert(received_by_rule(s1));
        static_assert(received_by_rule(s2));

        // 端口非法值
        // constexpr auto s3 = "server:65536"_span;     // 超最大端口
        constexpr auto s4 = "localhost:123abc"_span; // 非数字端口
        constexpr auto s5 = "host:-80"_span;         // 负号

        static_assert(received_by_rule(s4));
        static_assert(received_by_rule(s5));

        // 结构错误
        constexpr auto s6 = ":8080"_span; // 空 pseudonym
        // constexpr auto s7 = "host:"_span;    // 空端口
        constexpr auto s8 = "host::80"_span; // 多个冒号
        static_assert(not received_by_rule(s6));

        static_assert(received_by_rule(s8));
    }

    return 0;
}
// NOLINTEND