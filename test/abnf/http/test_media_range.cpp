#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 通用匹配 "*/*"
        constexpr auto s1 = "*/*"_span;
        constexpr auto s2 = "*/* ; q=0.8"_span;
        static_assert(media_range(s1));
        static_assert(media_range(s2));

        // 类型通配 "type/*"
        constexpr auto s3 = "text/*"_span;
        constexpr auto s4 = "application/*; version=1"_span;
        static_assert(media_range(s3));
        static_assert(media_range(s4));

        // 具体类型 "type/subtype"
        constexpr auto s5 = "text/plain"_span;
        constexpr auto s6 = "application/json; charset=utf-8"_span;
        static_assert(media_range(s5));
        static_assert(media_range(s6));

        // 复杂子类型（含保留字符）
        constexpr auto s7 = "application/vnd.api+json"_span;
        constexpr auto s8 = "x._~+/x._~+; param=~value"_span;
        static_assert(media_range(s7));
        static_assert(media_range(s8));

        // 极长输入
        constexpr auto s9 = "abcdefghijklmnopqrstuvwxyz0123456789-._~+/"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~+;param=value"_span;
        static_assert(media_range(s9));
    }
    // 非法测试用例
    {
        // 非法通配符位置
        constexpr auto s1 = "*/text"_span;       // 错误类型通配
        constexpr auto s2 = "text/*/plain"_span; // 多余斜杠
        static_assert(media_range(s1));
        {
            static_assert(type("*"_span));
        }
        static_assert(not media_range(s2));

        // 非法字符
        constexpr auto s3 = "text/@plain"_span;       // 子类型非法
        constexpr auto s4 = "app=lication/json"_span; // 类型非法
        static_assert(not media_range(s3));
        static_assert(not media_range(s4));

        // 结构错误
        constexpr auto s5 = "text"_span;                  // 缺少子类型
        constexpr auto s6 = "text/plain;key=val;ue"_span; // 参数非法
        static_assert(not media_range(s5));
        static_assert(not media_range(s6));

        // 空参数段
        constexpr auto s7 = "image/*; ; param=1"_span; // 中间空参数段
        static_assert(media_range(s7));
        {
            static_assert(parameters("; ; param=1"_span));
        }
    }
    // 边界测试
    {
        // 最小合法格式
        constexpr auto s1 = "a/b"_span;
        constexpr auto s2 = "*/*"_span;
        static_assert(media_range(s1));
        static_assert(media_range(s2));

        // 带空格的参数
        constexpr auto s3 = "text/html ; q=0.5 ; charset=\"utf-8\""_span;
        static_assert(media_range(s3));

        // 通配符与参数混合
        constexpr auto s4 = "text/*; q=0; flag"_span; // flag 视为空参数段（非法）
        static_assert(not media_range(s4));
    }

    return 0;
}
// NOLINTEND