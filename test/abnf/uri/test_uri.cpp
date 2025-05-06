
#include "../test_abnf.hpp"
#include "./test_uri.hpp"
#include <cassert>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;

#include <iostream>

int main()
{
    // 有效测试用例
    {
        // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        constexpr auto URI_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = URI{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 全要素组合测试
        constexpr auto full_uri =
            "https://user:pass@[v1.a]:80/path?q=1#frag"_span; // 所有组件
        constexpr auto encoded_uri =
            "scheme%2E:/a%20b?%23=%26#%25"_span; // 全组件编码测试

        static_assert(URI_rule(full_uri));

        // NOTE: scheme 没有百分号
        //  static_assert(URI_rule(encoded_uri));
        {
            auto s = scheme{};
            auto span = "scheme%2E"_span;
            auto ctx = make_parser_ctx(span);
            assert(s(ctx));
            assert(ctx.cur_index == "scheme"_span.size());
        }

        // 各hier-part类型覆盖
        constexpr auto URI_uri = "ftp://example.com/file.txt"_span;  // 类型1 hier-part
        constexpr auto absolute_uri = "file:/usr/bin/sh"_span;       // 类型2 hier-part
        constexpr auto rootless_uri = "test:segment:123?x=1#y"_span; // 类型3 hier-part
        constexpr auto empty_hier_uri = "m:#fragment"_span;          // 类型4 hier-part
        static_assert(URI_rule(URI_uri));
        static_assert(URI_rule(absolute_uri));
        static_assert(URI_rule(rootless_uri));
        static_assert(URI_rule(empty_hier_uri));

        // 边界/极端值测试
        constexpr auto max_port_uri = "http://host:65535/path"_span; // 最大合法端口
        constexpr auto minimal_uri = "a:b"_span;                     // 最小合法uri
        constexpr auto empty_query_frag = "s:/?#"_span;              // 空查询+空片段
        static_assert(URI_rule(max_port_uri));
        static_assert(URI_rule(minimal_uri));

        static_assert(URI_rule(empty_query_frag));

        // 特殊字符处理
        constexpr auto delims_in_components =
            "s://!$&'()*+,;=@host/path?/?#/?"_span; // 各组件允许的特殊字符
        static_assert(URI_rule(delims_in_components));

        constexpr auto tricky_encoding =
            "s://%2575ser@host?%3D%26#%25%23"_span; // 双重编码测试
        static_assert(URI_rule(tricky_encoding));

        constexpr auto double_colon = "scheme::path"_span; // 多个冒号
        static_assert(URI_rule(double_colon));
        {
            // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
            hier_part hier{};
            auto sp = ":path"_span;
            auto ctx = make_parser_ctx(sp);
            hier(ctx);
            assert(ctx.empty());
            assert(ctx.cur_index == sp.size());
            {
                path_rootless p; // pchar包含 ":"
                auto ctx = make_parser_ctx(sp);
                p(ctx);
                assert(ctx.empty());
                assert(ctx.cur_index == sp.size());
            }
        }

        constexpr auto fragment_before_query = "s:/p#frag?query"_span; // 片段在查询前
        static_assert(URI_rule(fragment_before_query));
        {
            hier_part hier{};
            auto sp = "/p"_span;
            auto ctx = make_parser_ctx(sp);
            auto ret = hier(ctx);
            assert(ret);
            std::cout << "ctx.cur_index: " << ctx.cur_index << '\n';
            assert(ctx.empty());
            assert(ctx.cur_index == sp.size());
            {
                fragment f{};
                auto sp = "frag?query"_span;
                auto ctx = make_parser_ctx(sp);
                auto ret = f(ctx);
                assert(ret);
                assert(ctx.empty());
                assert(ctx.cur_index == sp.size());
            }
        }

        constexpr auto uri_without_hier =
            "mailto:user@domain.com"_span;                   // mailto属于uri但不是URL
        constexpr auto urn_like = "urn:issn:1535-3613"_span; // URN格式（需特殊处理）

        static_assert(URI_rule(uri_without_hier)); // 根据RFC3986应为合法，需确认需求
        static_assert(URI_rule(urn_like));         // 根据RFC应为合法，需确认解析范围

        constexpr auto ambiguous_colon =
            "s:path:with:colons?q=1:2"_span; // 路径中的多个冒号
        static_assert(URI_rule(ambiguous_colon));
    }
    {
        // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        constexpr auto URI_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = URI{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };
        // 结构错误
        constexpr auto no_scheme = "//host/path"_span; // 缺少scheme和冒号

        // 非法字符渗透
        constexpr auto space_in_scheme = "sch eme:/path"_span;  // scheme含空格
        constexpr auto unencoded_space = "s:/a path#frag"_span; // hier-part未编码空格
        constexpr auto control_char = "s:/pa\x01th?q=1"_span;   // 控制字符

        // 组件格式错误
        constexpr auto invalid_port = "http://host:80port/path"_span; // 端口含字母
        constexpr auto bad_path_rootless = "s:seg ment"_span;    // rootless路径未编码空格
        constexpr auto illegal_fragment_char = "s:/#frag%"_span; // 片段含未编码%

        static_assert(not URI_rule(no_scheme));

        static_assert(!URI_rule(space_in_scheme));
        static_assert(URI_rule(unencoded_space));
        static_assert(URI_rule(control_char));
        static_assert(URI_rule(invalid_port));
        static_assert(URI_rule(bad_path_rootless));
        static_assert(URI_rule(illegal_fragment_char));

        // 特殊说明测试
        constexpr auto empty_components = "s:? # "_span; // 空hier-part+查询+片段
        static_assert(URI_rule(empty_components));
    }
    { // build
        constexpr auto URI_rule = URI{};
        {
            // 全要素组合测试
            constexpr auto full_uri =
                "https://user:pass@[v1.a]:80/path?q=1#frag"_span; // 所有组件
            auto ctx = make_parser_ctx(full_uri);
            auto ret = URI_rule.parse(ctx);
            std::cout << "ctx.cur_index: " << ctx.cur_index
                      << ", full_uri.size(): " << full_uri.size() << '\n';
            assert(ctx.cur_index = full_uri.size());
            assert(URI::buildString(*ret) ==
                   std::string("https://user:pass@[v1.a]:80/path?q=1#frag"));
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
