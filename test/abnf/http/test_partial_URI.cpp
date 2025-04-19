#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 仅有 relative-part（空路径）
        constexpr auto s1 = ""_span; // 空路径（需确认是否合法）
        constexpr auto s2 = "/"_span;
        static_assert(partial_URI(s1));
        static_assert(partial_URI(s2));

        // 路径 + 查询
        constexpr auto s3 = "/path?query=123"_span;
        constexpr auto s4 = "//example.com/path/to?key=value"_span;
        static_assert(partial_URI(s3));
        static_assert(partial_URI(s4));

        // 含保留字符的路径和查询
        constexpr auto s5 = "/a!b$c&d;e=/?q=!$&'()*+,;="_span;
        static_assert(partial_URI(s5));

        // 极长路径和查询
        // constexpr auto s6 =
        //     "/" + std::string(1000, 'a') + "?q=" + std::string(1000, 'b') _span;
        // static_assert(partial_URI(s6));
    }
    // 非法测试用例
    {
        // 非法路径字符（未编码）
        constexpr auto s1 = "/path with space"_span;
        constexpr auto s2 = "//example.com/bad^path"_span;
        static_assert(not partial_URI(s1));
        static_assert(not partial_URI(s2));

        // 非法查询字符
        constexpr auto s3 = "/search?<script>"_span;
        constexpr auto s4 = "/data?key=val#ue"_span; // # 属于片段
        static_assert(not partial_URI(s3));
        static_assert(not partial_URI(s4));

        // 结构错误
        constexpr auto s5 = "?query=alone"_span; // 缺少路径
        constexpr auto s6 = "/path??double=query"_span;
        static_assert(partial_URI(s5));
        static_assert(partial_URI(s6));
        {
            static_assert(relative_part("/path"_span));
            static_assert(query("?double=query"_span));
            constexpr auto ret = partial_URI(s6).value();

            constexpr auto span0 = tool::make_stdspan(s6, ret.get<0>());
            static_assert(tool::equal_span(span0, "/path"_span));
            static_assert(ret.get<1>() != invalid_span &&
                          tool::equal_span(tool::make_stdspan(s6, ret.get<1>()),
                                           "?double=query"_span));
        }
    }
    {
        // 空字符串（可能非法）
        constexpr auto s1 = ""_span;
        static_assert(partial_URI(s1)); // 根据 RFC 是否允许空

        // 最大合法长度（假设无限制）
        // constexpr auto s2 =
        //     "/" + std::string(10000, 'a') + "?" + std::string(10000, 'b') _span;
        // static_assert(not partial_URI(s2));
    }

    return 0;
}
// NOLINTEND