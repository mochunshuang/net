#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // t-codings = "trailers" / ( transfer-coding [ weight ] )
    // 合法测试用例
    {
        constexpr auto t_codings_rule = make_pass_test<t_codings>();
        // 纯 trailers
        constexpr auto s1 = "trailers"_span;
        static_assert(t_codings_rule(s1));

        // transfer-coding 不带 weight
        constexpr auto s2 = "gzip"_span;
        static_assert(t_codings_rule(s2));

        // transfer-coding 带 weight
        constexpr auto s3 = "deflate;q=0.5"_span;
        static_assert(t_codings_rule(s3));

        // 含 OWS 的 weight
        constexpr auto s4 = "br   ;   q=1.000"_span;
        static_assert(t_codings_rule(s4));

        // 边界 q 值
        constexpr auto s5 = "chunked;q=0"_span;      // q=0
        constexpr auto s6 = "compress;q=0.000"_span; // 三位小数
        static_assert(t_codings_rule(s5));
        static_assert(t_codings_rule(s6));

        //  trailers 大小写不敏感（若 RFC 允许）
        constexpr auto s7 = "Trailers"_span;
        static_assert(t_codings_rule(s7)); // 需确认 RFC 规则

        {
            // 无效 transfer-coding; 有效的
            constexpr auto s1 = "invalid;q=0.5"_span; // transfer-coding 非法
            static_assert(t_codings_rule(s1));

            static_assert(make_pass_test<transfer_coding>()("invalid"_span));
            static_assert(make_pass_test<weight>()(";q=0.5"_span));
        }
        {
            // NOTE:多参数。 q2 变成 q 将错误。目的是为了 后面的 wight 规则做出下限制
            constexpr auto s7 = "chunked;q2=0.5;param=123"_span;
            static_assert(t_codings_rule(s7));
        }
    }
    // 非法测试用例
    {
        constexpr auto t_codings_rule = make_unpass_test<t_codings>();

        // trailers 带参数
        constexpr auto s2 = "trailers;q=0.5"_span; // trailers 不允许 weight
        static_assert(t_codings_rule(s2));

        // 格式错误（缺少分号）
        constexpr auto s3 = "gzip q=0.5"_span;
        static_assert(t_codings_rule(s3));

        // 非法 q 值

        constexpr auto s6 = "deflate;q="_span; // 空值

        static_assert(t_codings_rule(s6));

        // 非 trailers 的无效格式
        constexpr auto s8 = "gzip;"_span; // 缺少 weight
        static_assert(t_codings_rule(s8));

        {
            // t-codings = "trailers" / ( transfer-coding [ weight ] )
            constexpr auto s4 = "br;q=1.5"_span;   // q > 1
            constexpr auto s5 = "gzip;q=abc"_span; // 非数字
            static_assert(t_codings_rule(s4));
            {
                {
                    // NOTE: 优先级问题。 目前通过不允许 token 是 q 来解决
                    //  TODO : BUG错误原因，;被当作 transfer_coding
                    //  transfer-coding = token *( OWS ";" OWS transfer-parameter )
                    //  transfer-parameter = token BWS "=" BWS ( token / quoted-string )
                    // static_assert(make_pass_test<transfer_coding>()("br;q=1.5"_span));
                    // static_assert(make_pass_test<transfer_parameter>()("q=1.5"_span));
                }
                constexpr auto ret = make_rule_test<weight>(";q=1.5"_span);
                static_assert(ret.first);
                static_assert(not ret.second.done());
                {
                    // NOTE: 确实是非法的
                    static_assert(make_unpass_test<qvalue>()("1.5"_span));
                }
            }
            static_assert(t_codings_rule(s5));

            // 多余参数
            constexpr auto s7 = "chunked;q=0.5;param=123"_span;
            static_assert(t_codings_rule(s7));
        }
    }

    return 0;
}
// NOLINTEND