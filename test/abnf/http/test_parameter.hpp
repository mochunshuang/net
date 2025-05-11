#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto parameter = make_pass_test<mcs::abnf::http::parameter>();
    constexpr auto not_parameter = make_unpass_test<mcs::abnf::http::parameter>();
    // 合法测试用例
    {
        // 基础合法格式
        constexpr auto s1 = "key=value"_span;
        constexpr auto s2 = "version=1.0"_span;
        static_assert(parameter(s1));
        static_assert(parameter(s2));

        // 特殊字符参数名/值（根据规则）
        constexpr auto s3 = "custom~key=!$&'()*+,;="_span;
        constexpr auto s4 = "flag+=~/_encoded"_span;
        static_assert(not_parameter(s3));
        {
            static_assert(
                make_pass_test<mcs::abnf::http::parameter_name>()("custom~key"_span));
            static_assert(
                make_unpass_test<mcs::abnf::http::parameter_value>()("!$&'()*+,;="_span));
            {
                static_assert(not make_unpass_test<mcs::abnf::http::tchar>()("="_span));
            }
        }
        static_assert(not_parameter(s4));
        {
            static_assert(
                make_pass_test<mcs::abnf::http::parameter_name>()("flag+"_span));
            static_assert(
                make_unpass_test<mcs::abnf::http::parameter_value>()("~/_encoded"_span));
            {
                static_assert(not make_unpass_test<mcs::abnf::http::tchar>()("/"_span));
            }
        }

        // 带空格的参数值（需 quoted-string）
        constexpr auto s5 = "msg=\"hello world\""_span;
        static_assert(parameter(s5));

        // 极长参数名/值
        constexpr auto s6 =
            "abcdefghijklmnopqrstuvwxyz0123456789-._~=ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~"_span;
        static_assert(parameter(s6));
    }
    return 0;
}
// NOLINTEND