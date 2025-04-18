#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf::http;

void test_transfer_coding()
{
    // 有效测试用例
    {
        // 仅有 token
        constexpr auto s1 = "chunked"_span;
        {
            static_assert(token(s1));
        }
        static_assert(transfer_coding(s1));

        // 带一个参数
        constexpr auto s2 = "gzip;q=1"_span;
        {
            static_assert(token("gzip"_span));
            static_assert(transfer_parameter("q=1"_span));
        }
        static_assert(transfer_coding(s2));

        // 参数含 OWS
        constexpr auto s3 = "deflate ; q=0.5 ; version=1"_span;
        static_assert(transfer_coding(s3));

        // 参数值为 quoted-string
        constexpr auto s4 = "compress;name=\"example\""_span;
        static_assert(transfer_coding(s4));

        // 带转义的 quoted-string
        constexpr auto s5 = "format;text=\"quote\\\"here\""_span;
        static_assert(transfer_coding(s5));

        // 混合 token 和 quoted-string
        constexpr auto s6 = "br;level=5;param=\"v;1.0\""_span;
        static_assert(transfer_coding(s6));
    }

    // 无效测试用例
    {
        // 参数缺少等号
        constexpr auto s1 = "gzip;q"_span;
        assert(not transfer_coding(s1));

        // 引号未闭合
        constexpr auto s2 = "err;name=\"test"_span;
        static_assert(not transfer_coding(s2));

        // 非法转义字符
        constexpr auto s3 = "err;str=\"\\x\""_span;
        {
            static_assert(transfer_parameter("str=\"\\x\""_span));
        }
        static_assert(transfer_coding(s3));

        // token 含非法字符（空格）
        constexpr auto s4 = "in valid;q=1"_span;
        static_assert(not transfer_coding(s4));

        // 分号后无参数
        constexpr auto s5 = "chunked;"_span;
        static_assert(not transfer_coding(s5));

        // 参数值含非法字符（非token/quoted-string）
        constexpr auto s6 = "err;q=!"_span;
        {
            static_assert(transfer_parameter("q=!"_span));
        }
        static_assert(transfer_coding(s6));

        // 等号后无值（越界检查）
        constexpr auto s7 = "err;q= "_span;
        assert(not transfer_coding(s7));
    }

    // 边界测试
    {
        // 空输入（应当失败）
        constexpr auto s1 = ""_span;
        static_assert(not transfer_coding(s1));

        // 允许的特殊符号（如'+', '/'等）
        constexpr auto s2 = "custom+coding;param=~/path"_span;
        {
            static_assert(token("custom+coding"_span));
            static_assert(not transfer_parameter("param=~/path"_span));
        }
        static_assert(not transfer_coding(s2));

        // quoted-string中的换行符（obs-text，需根据实现判断是否支持）
        constexpr auto s3 = "test;msg=\"hello\x80\""_span;
        // 需要确认是否允许 obs-text（可能根据 __qdtext 实现）
        static_assert(transfer_coding(s3));
    }
}

int main()
{
    test_transfer_coding();
    return 0;
}
// NOLINTEND