
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
        constexpr auto scheme_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::scheme{}(ctx);
            assert(ctx.empty());
            assert(ctx.cur_index == ctx.root_span.size());
            return suc;
        };
        // 基本合法格式
        constexpr auto basic_scheme = "http"_span; // 纯小写字母
        constexpr auto mixed_case_scheme =
            "FTP"_span; // 大写开头混合大小写（规范允许但推荐小写）
        constexpr auto with_numbers = "s3"_span; // 字母+数字
        constexpr auto with_special_chars =
            "my+app-1.0"_span;                 // 包含所有允许符号（+ - .）
        constexpr auto single_char = "x"_span; // 最小长度
        constexpr auto max_length = "a1234567890.+-.zzz"_span; // 长合法组合

        static_assert(scheme_rule(detail::make_parser_ctx(basic_scheme)));
        static_assert(scheme_rule(detail::make_parser_ctx(mixed_case_scheme)));
        static_assert(scheme_rule(detail::make_parser_ctx(with_numbers)));
        static_assert(scheme_rule(detail::make_parser_ctx(with_special_chars)));
        static_assert(scheme_rule(detail::make_parser_ctx(single_char)));
        static_assert(scheme_rule(detail::make_parser_ctx(max_length)));
    }
    {
        constexpr auto scheme_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::scheme{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        // 非法首字符
        constexpr auto starts_with_number = "3d-printer"_span; // 首字符非ALPHA
        constexpr auto starts_with_dot = ".hidden"_span;       // 首字符为点号
        constexpr auto starts_with_plus = "+stream"_span;      // 首字符为+
        static_assert(!scheme_rule(detail::make_parser_ctx(starts_with_number)));
        static_assert(!scheme_rule(detail::make_parser_ctx(starts_with_dot)));
        static_assert(!scheme_rule(detail::make_parser_ctx(starts_with_plus)));

        // 包含非法字符
        constexpr auto has_underscore = "my_scheme"_span; // 下划线非法
        constexpr auto has_space = "git hub"_span;        // 包含空格
        constexpr auto has_percent =
            "auth%20"_span; // 含百分号（应使用pct-encoded在其他部分）
        {
            constexpr auto scheme_rule = [](parser_ctx ctx) constexpr {
                assert(ctx.cur_index == 0);
                auto suc = mcs::abnf::uri::scheme{}(ctx);
                assert(ctx.cur_index != ctx.root_span.size());
                return suc;
            };
            static_assert(scheme_rule(detail::make_parser_ctx(has_underscore)));
            static_assert(scheme_rule(detail::make_parser_ctx(has_underscore)).value() ==
                          2);

            static_assert(scheme_rule(detail::make_parser_ctx(has_space)));
            static_assert(scheme_rule(detail::make_parser_ctx(has_space)).value() == 3);

            static_assert(scheme_rule(detail::make_parser_ctx(has_percent)));
            static_assert(scheme_rule(detail::make_parser_ctx(has_percent)).value() == 4);
        }

        // 结构错误
        constexpr auto empty_string = ""_span;          // 空字符串
        constexpr auto only_special = "+-."_span;       // 无起始字母
        constexpr auto trailing_colon = "scheme:"_span; // 包含URI分隔符（:）
        static_assert(!scheme_rule(detail::make_parser_ctx(empty_string)));
        static_assert(!scheme_rule(detail::make_parser_ctx(only_special)));
        {
            constexpr auto scheme_rule = [](parser_ctx ctx) constexpr {
                assert(ctx.cur_index == 0);
                auto suc = mcs::abnf::uri::scheme{}(ctx);
                assert(ctx.cur_index != ctx.root_span.size());
                return suc;
            };
            static_assert(scheme_rule(detail::make_parser_ctx(trailing_colon)));
            static_assert(scheme_rule(detail::make_parser_ctx(trailing_colon)).value() ==
                          6);
        }

        // 极端用例：看似合法但含隐藏错误
        constexpr auto unicode_char = "utf✓8"_span;    // 含Unicode字符（✓不属于ALPHA）
        constexpr auto control_char = "ht\x01tp"_span; // 含控制字符
        {
            constexpr auto scheme_rule = [](parser_ctx ctx) constexpr {
                assert(ctx.cur_index == 0);
                auto suc = mcs::abnf::uri::scheme{}(ctx);
                assert(ctx.cur_index != ctx.root_span.size());
                return suc;
            };
            static_assert(scheme_rule(detail::make_parser_ctx(unicode_char)));
            static_assert(scheme_rule(detail::make_parser_ctx(control_char)));
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
