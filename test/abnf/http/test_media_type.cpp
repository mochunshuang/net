#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 基础格式
        constexpr auto s1 = "text/plain"_span;
        constexpr auto s2 = "application/json; charset=utf-8"_span;
        static_assert(media_type(s1));
        {
            //  media-type = type "/" subtype parameters
            // parameters = *( OWS ";" OWS [ parameter ] )
            static_assert(parameters(""_span));
            constexpr auto ret = media_type(s1).value();

            static_assert(decltype(ret)::size() == 3);
            static_assert(
                tool::equal_span(tool::make_stdspan(s1, ret.get<0>()), "text"_span));
            static_assert(
                tool::equal_span(tool::make_stdspan(s1, ret.get<1>()), "plain"_span));
            static_assert(ret.get<2>() == invalid_span);
        }
        static_assert(media_type(s2));
        {
            constexpr auto ret = media_type(s2).value();
            static_assert(tool::equal_span(tool::make_stdspan(s2, ret.get<2>()),
                                           "; charset=utf-8"_span));
        }

        // 特殊字符类型/子类型（根据 token 规则）
        constexpr auto s3 = "vnd.custom+type/sub_type~v1"_span;
        constexpr auto s4 = "x._~+/x._~+"_span;
        static_assert(media_type(s3));
        static_assert(media_type(s4));

        // 带多参数的复杂用例
        constexpr auto s5 = "multipart/form-data; boundary=----ABC; charset=utf-8"_span;
        static_assert(media_type(s5));
    }
    {
        // 缺少斜杠
        constexpr auto s1 = "textplain"_span;
        constexpr auto s2 = "application/"_span; // 缺少子类型
        static_assert(not media_type(s1));
        static_assert(not media_type(s2));

        // 非法字符
        constexpr auto s3 = "text/plain; key=va|lue"_span; // 管道符非法
        constexpr auto s4 = "app@cation/json"_span;        // @符号非法
        static_assert(media_type(s3));
        {
            static_assert(parameters("; key=va|lue"_span));
        }
        static_assert(not media_type(s4));

        // 结构错误
        constexpr auto s5 = "text/plain;key=value;extra"_span; // 无效参数
        constexpr auto s6 = "video/mp4; =no-name"_span;        // 空参数名
        static_assert(not media_type(s5));
        static_assert(not media_type(s6));
    }
    // 边界测试
    {
        // 极长类型/子类型
        constexpr auto s1 = "abcdefghijklmnopqrstuvwxyz0123456789-._~+/"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~+"_span;
        static_assert(media_type(s1));

        // 空参数部分
        constexpr auto s2 = "text/html;"_span; // 允许尾随分号
        static_assert(media_type(s2));

        // 最小合法格式
        constexpr auto s3 = "a/b"_span;
        static_assert(media_type(s3));
    }

    return 0;
}
// NOLINTEND