#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 纯 trailers
        constexpr auto s1 = "trailers"_span;
        static_assert(t_codings(s1));

        // transfer-coding 不带 weight
        constexpr auto s2 = "gzip"_span;
        static_assert(t_codings(s2));

        // transfer-coding 带 weight
        constexpr auto s3 = "deflate;q=0.5"_span;
        static_assert(t_codings(s3));

        // 含 OWS 的 weight
        constexpr auto s4 = "br   ;   q=1.000"_span;
        static_assert(t_codings(s4));

        // 边界 q 值
        constexpr auto s5 = "chunked;q=0"_span;      // q=0
        constexpr auto s6 = "compress;q=0.000"_span; // 三位小数
        static_assert(t_codings(s5));
        static_assert(t_codings(s6));

        // TODO ABNF默认是不区分大小写的. 要改代码
        //  trailers 大小写不敏感（若 RFC 允许）
        //  constexpr auto s7 = "Trailers"_span;
        //  static_assert(t_codings(s7)); // 需确认 RFC 规则
    }
    // 非法测试用例
    {
        // 无效 transfer-coding
        constexpr auto s1 = "invalid;q=0.5"_span; // transfer-coding 非法
        static_assert(t_codings(s1));

        // trailers 带参数
        constexpr auto s2 = "trailers;q=0.5"_span; // trailers 不允许 weight
        static_assert(t_codings(s2));
        {
            // NOTE: 允许吗？
            static_assert(transfer_coding(s2));
        }

        // 格式错误（缺少分号）
        constexpr auto s3 = "gzip q=0.5"_span;
        static_assert(not t_codings(s3));

        // 非法 q 值
        constexpr auto s4 = "br;q=1.5"_span;   // q > 1
        constexpr auto s5 = "gzip;q=abc"_span; // 非数字
        constexpr auto s6 = "deflate;q="_span; // 空值
        static_assert(t_codings(s4));
        {
            static_assert(transfer_coding(s4)); // 但是
            static_assert(not weight(";q=1.5"_span));
            // NOTE: 结构化绑定还不能 constexpr
            auto [a, b] = t_codings(s4).value();
            assert(b == invalid_span); // 没有 weight
        }
        static_assert(t_codings(s5));
        {
            static_assert(transfer_coding(s5));
        }
        assert(not t_codings(s6));

        // 多余参数
        constexpr auto s7 = "chunked;q=0.5;param=123"_span;
        static_assert(t_codings(s7));

        // 非 trailers 的无效格式
        constexpr auto s8 = "gzip;"_span; // 缺少 weight
        static_assert(not t_codings(s8));
    }

    return 0;
}
// NOLINTEND