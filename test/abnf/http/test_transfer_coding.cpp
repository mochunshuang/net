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

        // 带一个参数
        constexpr auto s2 = "gzip;q=1"_span;

        // 参数含 OWS //NOTE: q=0.5 做 weight 的，q是关键字符，必须不允许被前面的占用
        constexpr auto s3 = "deflate ; q=0.5 ; version=1"_span;

        // 参数值为 quoted-string
        constexpr auto s4 = "compress;name=\"example\""_span;

        // 带转义的 quoted-string
        constexpr auto s5 = "format;text=\"quote\\\"here\""_span;

        // 混合 token 和 quoted-string
        constexpr auto s6 = "br;level=5;param=\"v;1.0\""_span;

        // transfer-coding = token *( OWS ";" OWS transfer-parameter )
        // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
        constexpr auto transfer_coding_rule = make_pass_test<transfer_coding>();
        static_assert(transfer_coding_rule(s1));
        // NOTE: transfer_parameter 不能使用 q 做token了
        //  static_assert(transfer_coding_rule(s2));
        //  static_assert(transfer_coding_rule(s3));
        static_assert(transfer_coding_rule(s4));
        static_assert(transfer_coding_rule(s5));
        static_assert(transfer_coding_rule(s6));

        {
            // 非法转义字符
            constexpr auto s3 = "err;str=\"\\x\""_span;
            static_assert(transfer_coding_rule(s3));
            {
                constexpr auto ret =
                    make_rule_test<transfer_parameter>("str=\"\\x\""_span);
                static_assert(ret.first.has_value());
                static_assert(ret.second.done());
            }
        }
        {
            // 参数值含非法字符（非token/quoted-string）
            constexpr auto s6 = "err;q2=!"_span;
            static_assert(transfer_coding_rule(s6));
            {
                constexpr auto ret = make_rule_test<transfer_parameter>("q2=!"_span);
                static_assert(ret.first.has_value());
                static_assert(ret.second.done());
            }
        }
    }

    // 无效测试用例
    {
        constexpr auto transfer_coding_rule = make_unpass_test<transfer_coding>();

        // 参数缺少等号
        constexpr auto s1 = "gzip;q"_span;
        static_assert(transfer_coding_rule(s1));

        // 引号未闭合
        constexpr auto s2 = "err;name=\"test"_span;
        static_assert(transfer_coding_rule(s2));

        // token 含非法字符（空格）
        constexpr auto s4 = "in valid;q=1"_span;
        static_assert(transfer_coding_rule(s4));

        // 分号后无参数
        constexpr auto s5 = "chunked;"_span;

        // 等号后无值（越界检查）
        constexpr auto s7 = "err;q= "_span;

        static_assert(transfer_coding_rule(s5));
        static_assert(transfer_coding_rule(s7));
    }

    // 边界测试
    {
        // 空输入（应当失败）
        constexpr auto s1 = ""_span;

        constexpr auto transfer_coding_rule = make_pass_test<transfer_coding>();
        static_assert(not transfer_coding_rule(s1));

        // 允许的特殊符号（如'+', '/'等）
        constexpr auto s2 = "custom+coding;param=~/path"_span;
        {
            constexpr auto transfer_coding_rule = make_unpass_test<transfer_coding>();
            static_assert(transfer_coding_rule(s2));
            {
                constexpr auto ret =
                    make_rule_test<transfer_parameter>("param=~/path"_span);
                static_assert(ret.first.has_value());
                static_assert(!ret.second.done());
            }
        }

        // quoted-string中的换行符（obs-text，需根据实现判断是否支持）
        constexpr auto s3 = "test;msg=\"hello\x80\""_span;
        // 需要确认是否允许 obs-text（可能根据 __qdtext 实现）
        static_assert(transfer_coding_rule(s3));
    }
}

int main()
{
    test_transfer_coding();
    return 0;
}
// NOLINTEND