#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Accept = [ ( media-range [ weight ] ) *( OWS "," OWS ( media-range [ weight ] ) ) ]
    constexpr auto accept_pass = make_pass_test<mcs::abnf::http::Accept>();
    constexpr auto accept_fail = make_unpass_test<mcs::abnf::http::Accept>();

    static_assert(accept_pass("application/json;q=0.9"_span)); // 多类型带权重
    static_assert(accept_pass("*/*"_span));                    // 通配类型

    static_assert(accept_fail("text/html;q=1.1"_span)); // 权重越界
    {
        // NOTE: media_range 全部占用了，又是 q 特殊吗 来解决吗
        // // parameters = *( OWS ";" OWS [ parameter ] )
        static_assert(
            make_pass_test<mcs::abnf::http::media_range>()("text/html;q=1.1"_span));

        static_assert(not make_unpass_test<mcs::abnf::http::weight>()("q=1.1"_span));

        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()("text/html"_span));
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()("*/html"_span));
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()("*/*"_span));

        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()("text/html;"_span));
        {
            // parameter_name \ parameter_value
            static_assert(make_pass_test<mcs::abnf::http::parameter_name>()("abc"_span));
            static_assert(make_pass_test<mcs::abnf::http::parameter_value>()("abc"_span));
            static_assert(make_pass_test<mcs::abnf::http::parameter_value>()(
                "\"abc ada asdasd\""_span));
            static_assert(make_pass_test<mcs::abnf::http::parameter_value>()(
                "\"abc ada asdasd\""_span));
        }
        static_assert(
            make_pass_test<mcs::abnf::http::AcceptRule>()("text/html;abc=cde"_span));
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;abc=\"abc ada asdasd\""_span));
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;abc=\"abc ada asdasd\";efd=123"_span));

        // [wight]
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;abc=\"abc ada asdasd\";efd=123;q=0.001"_span));
        static_assert(make_unpass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;abc=\"abc ada asdasd\";efd=123;q=1.001"_span)); // NOTE: 失败

        // 多个 media-range [ weight ]
        static_assert(
            make_pass_test<mcs::abnf::http::AcceptRule>()("text/html;q=0.001,"_span));
        static_assert(
            make_pass_test<mcs::abnf::http::AcceptRule>()("text/html;q=0.001,"_span));
        {
            constexpr auto ret = make_rule_test<mcs::abnf::http::qvalue>("0.001"_span);
            static_assert(ret.first.has_value());
            static_assert(ret.first.value() == 5);
        }
        {
            constexpr auto ret =
                make_rule_test<mcs::abnf::http::qvalue>("0.001  ,    "_span);
            static_assert(ret.first.has_value());
            static_assert(ret.first.value() == 5);
        }

        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;q=0.001,text/html"_span));
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;q=0.001,text/html;,text/html;abc=\"abc ada asdasd\";efd=123;q=0.001"_span));

        // 空格
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;q=0.001 ,text/html;,text/html;abc=\"abc ada asdasd\";efd=123;q=0.001"_span));
        static_assert(make_pass_test<mcs::abnf::http::AcceptRule>()(
            "text/html;q=0.001 ,      text/html;  ,    text/html;abc=\"abc ada asdasd\";efd=123;q=0.001"_span));
    }
    // 测试权重边界值
    // static_assert(accept_pass("text/html;q=0"_span));     // 允许0
    // static_assert(accept_pass("text/html;q=1"_span));     // 允许1
    // static_assert(accept_pass("text/html;q=1.001"_span)); // 超上限

    // 测试媒体类型通配符组合
    static_assert(accept_pass("image/*"_span)); // 部分通配
    static_assert(accept_pass("*/json"_span));  // 非常规但合规
    static_assert(accept_fail("*/"_span));      // 不完整类型

    static_assert(accept_fail("application/"_span)); // 不完整类型

    return 0;
}
// NOLINTEND