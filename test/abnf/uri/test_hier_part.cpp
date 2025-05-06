
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
        constexpr auto hier_part_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = hier_part{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 类型1：//authority + path-abempty
        constexpr auto type1_full =
            "//user:pass@[v1.a]:80/path/sub"_span;        // 完整authority和路径
        constexpr auto type1_min = "//example.com"_span;  // 最小authority（无路径）
        constexpr auto type1_empty_path = "//host:"_span; // 空port + 空路径

        // 类型2：path-absolute
        constexpr auto type2_basic = "/data/report.pdf"_span; // 标准绝对路径
        constexpr auto type2_traversal =
            "/../safe/../path"_span; // 包含..但符合语法（标准化前）
        constexpr auto type2_special = "/!$&'()*+,;=/_"_span; // 包含允许的特殊字符

        // 类型3：path-rootless
        constexpr auto type3_basic = "segment:123"_span;       // 无斜杠的根less路径
        constexpr auto type3_multi = "a/b/c;param"_span;       // 多段路径
        constexpr auto type3_encoded = "file%20name%3A1"_span; // 编码字符（空格和冒号）

        // 类型4：path-empty
        constexpr auto type4_empty = ""_span;            // 空字符串
        constexpr auto type4_with_scheme = "http:"_span; // 与scheme组合时的空路径

        static_assert(hier_part_rule(type1_full));
        static_assert(hier_part_rule(type1_min));
        static_assert(hier_part_rule(type1_empty_path));
        static_assert(hier_part_rule(type2_basic));
        static_assert(hier_part_rule(type2_traversal));
        static_assert(hier_part_rule(type2_special));
        static_assert(hier_part_rule(type3_basic));
        static_assert(hier_part_rule(type3_multi));
        static_assert(hier_part_rule(type3_encoded));
        static_assert(hier_part_rule(type4_empty));
        static_assert(hier_part_rule(type4_with_scheme));

        constexpr auto mixed_types =
            "//host/path/rootless"_span; // 类型1后接rootless路径（非法）
        constexpr auto rootless_start =
            "/rootless"_span; // 以/开头的rootless路径（应为path-absolute）
        static_assert(hier_part_rule(mixed_types));
        static_assert(hier_part_rule(rootless_start));

        constexpr auto empty_authority = "///path"_span;  // 空authority（三斜杠）
        constexpr auto colon_in_absolute = "/pa:th"_span; // path-absolute段含冒号

        static_assert(hier_part_rule(empty_authority));
        static_assert(hier_part_rule(colon_in_absolute));

        constexpr auto encoded_authority =
            "//%75ser@host.%65x%61mple"_span; // 全编码authority

        constexpr auto rootless_edge =
            "::::::::::"_span; // 全冒号（语法合法但语义可能无效）

        static_assert(hier_part_rule(encoded_authority)); // 验证编码解析
        static_assert(hier_part_rule(rootless_edge));     // 纯冒号符合path-rootless语法

        // 类型优先级测试
        constexpr auto empty_vs_absolute = "/"_span; // 应识别为path-absolute而非empty
        static_assert(hier_part_rule(empty_vs_absolute));
    }

    // 无效测试用例
    {
        constexpr auto hier_part_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = hier_part{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };
        // 非法字符
        constexpr auto illegal_in_authority = "//host^name/"_span; // authority含非法字符^
        constexpr auto unencoded_space = "/path with space"_span;  // 路径未编码空格
        constexpr auto invalid_rootless = "seg{ment}"_span;        // 含大括号非法字符

        // 格式错误
        constexpr auto bad_port = "//host:portnum"_span; // 非数字端口

        // 类型混淆
        constexpr auto empty_as_rootless = ""_span;      // 空字符串应属于path-empty
        constexpr auto rootless_with_query = "a?b"_span; // 查询组件属于上层URI结构

        static_assert(hier_part_rule(illegal_in_authority));
        static_assert(hier_part_rule(unencoded_space));
        static_assert(hier_part_rule(invalid_rootless));

        static_assert(hier_part_rule(bad_port));

        static_assert(hier_part_rule(empty_as_rootless));
        static_assert(hier_part_rule(rootless_with_query));
    }

    // build 测试
    {
        constexpr auto hier_part_rule = hier_part{};
        {
            // 类型优先级测试
            constexpr auto empty_vs_absolute = "/"_span; // 应识别为path-absolute而非empty
            auto ctx = make_parser_ctx(empty_vs_absolute);
            auto ret = hier_part_rule.parse(ctx);
            assert(ret);
            auto &result = *ret;
            assert(result.value.index() == 2);
            assert(std::holds_alternative<hier_part::result_type::path_absolute_t>(
                result.value));
        }
        {
            // 类型1：//authority + path-abempty
            constexpr auto type1_full =
                "//user:pass@[v1.a]:80/path/sub"_span; // 完整authority和路径
            auto ctx = make_parser_ctx(type1_full);
            auto ret = hier_part_rule.parse(ctx);
            assert(ret);
            auto &result = *ret;
            assert(std::holds_alternative<hier_part::result_type::authority_path_t>(
                result.value));
            assert(hier_part::buildString(*ret) ==
                   std::string("//user:pass@[v1.a]:80/path/sub"));
        }
        {
            // empty
            constexpr auto empty_as_rootless = ""_span; // 空字符串应属于path-empty
            auto ctx = make_parser_ctx(empty_as_rootless);
            auto ret = hier_part_rule.parse(ctx);
            assert(ret);
            assert(std::holds_alternative<hier_part::result_type::path_empty_t>(
                (*ret).value));
            assert(hier_part::buildString(*ret) == std::string(""));
            {
                auto path_empty_rule = path_empty{};
                auto ctx = make_parser_ctx(empty_as_rootless);
                path_empty_rule.parse(ctx);
                assert(ret.has_value());
            }
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
