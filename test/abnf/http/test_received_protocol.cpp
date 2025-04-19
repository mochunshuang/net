#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // 合法测试用例
    {
        // 仅 protocol-version
        constexpr auto s1 = "HTTP/1.1"_span; // 完整格式
        constexpr auto s2 = "1.1"_span;      // 仅 version
        static_assert(received_protocol(s1));
        {
            constexpr auto ret = received_protocol(s1).value();
            constexpr auto span0 = tool::make_stdspan(s1, ret.get<0>());
            static_assert(tool::equal_span(span0, "HTTP"_span));
            static_assert(
                ret.get<1>() != invalid_span &&
                tool::equal_span(tool::make_stdspan(s1, ret.get<1>()), "1.1"_span));
        }
        static_assert(received_protocol(s2));
        {
            constexpr auto ret = received_protocol(s2).value();
            static_assert(ret.get<0>() == invalid_span);
            static_assert(
                ret.get<1>() != invalid_span &&
                tool::equal_span(tool::make_stdspan(s2, ret.get<1>()), "1.1"_span));
        }

        // 特殊符号（根据 token 规则允许的字符）
        constexpr auto s3 = "custom+protocol/2.0~beta"_span;
        constexpr auto s4 = "v3.14"_span;
        static_assert(received_protocol(s3));
        static_assert(received_protocol(s4));

        // 边界长度
        constexpr auto s5 = "a/b"_span;                // 最短合法格式
        constexpr auto s6 = "ABCDEFGHIJ-._~+/=="_span; // 极长 token
        static_assert(received_protocol(s5));
        static_assert(not received_protocol(s6));
        {
            // ABCDEFGHIJ-._~+
            static_assert(protocol_name("ABCDEFGHIJ-._~+"_span));
            static_assert(not protocol_version("=="_span));
            static_assert(not tchar('='));
        }
    }
    // 非法测试用例
    {
        // 非法分隔符
        constexpr auto s1 = "HTTP\\1.1"_span;    // 反斜杠
        constexpr auto s2 = "name:version"_span; // 冒号
        static_assert(not received_protocol(s1));
        static_assert(not received_protocol(s2));

        // 非法字符
        constexpr auto s3 = "pro tocol/1.0"_span;   // 协议名含空格
        constexpr auto s4 = "name@domain/1.0"_span; // @符号非法
        static_assert(not received_protocol(s3));
        static_assert(not received_protocol(s4));

        // 结构错误
        constexpr auto s5 = "HTTP//1.1"_span; // 多斜杠
        constexpr auto s6 = "/1.0"_span;      // 空协议名
        constexpr auto s7 = "name/"_span;     // 空版本号
        static_assert(not received_protocol(s5));
        static_assert(not received_protocol(s6));
        static_assert(not received_protocol(s7));
    }
    // 边界测试
    {
        // 空输入
        constexpr auto s1 = ""_span;
        static_assert(not received_protocol(s1));

        // 仅斜杠
        constexpr auto s2 = "/"_span;
        static_assert(not received_protocol(s2));

        // 最大允许长度（假设实现无长度限制）
        constexpr auto s3 = "abcdefghijklmnopqrstuvwxyz0123456789-._~+/"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ=="_span; // 1024+字符
        static_assert(not received_protocol(s3));
        {
            static_assert(not tchar('='));
        }
    }
    {
        constexpr auto s1 = "HTTP|1.1"_span;
        constexpr auto ret1 = received_protocol(s1);

        constexpr auto s2 = "proto col/1.0"_span;
        constexpr auto ret2 = received_protocol(s2);
    }

    return 0;
}
// NOLINTEND