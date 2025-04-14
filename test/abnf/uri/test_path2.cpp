

#include "../test_head.hpp"

#include <array>
#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf::uri; // NOLINT
using OCTET = mcs::abnf::OCTET;

namespace mcs::abnf::uri::test
{
    // 辅助工具函数
    constexpr auto make_span2 = [](auto &&arr) {
        return std::span<const OCTET>{arr};
    };

    // ------------------------- 有效路径测试 -------------------------
    // path-empty
    static constexpr std::array<OCTET, 0> empty;

    static_assert(path(make_span2(empty)));

    // path-abempty (允许"//")
    static constexpr OCTET abempty1[] = {'/'};
    static constexpr OCTET abempty2[] = {'/', '/', 'a'};
    static constexpr OCTET abempty3[] = {'/', 'a', ':', 'b', '/', 'c'}; // 包含冒号
    static_assert(path(make_span2(abempty1)));
    static_assert(path(make_span2(abempty2)));
    static_assert(path(make_span2(abempty3)));

    // path-absolute (以"/"开头且非"//")
    static constexpr OCTET absolute1[] = {'/', 'a'};
    static constexpr OCTET absolute2[] = {'/', 'a', ':', 'b', '/', 'c'}; // 包含冒号
    static_assert(path(make_span2(absolute1)));
    static_assert(path(make_span2(absolute2)));

    // path-noscheme (非冒号起始)
    static constexpr OCTET noscheme1[] = {'a', '@', 'b', '/', 'c'}; // 含@符号
    static constexpr OCTET noscheme2[] = {'%', '4', '1', '/', 'd'}; // 编码字符
    static_assert(path(make_span2(noscheme1)));
    static_assert(path(make_span2(noscheme2)));

    // path-rootless (允许冒号)
    static constexpr OCTET rootless1[] = {'a', ':', 'b', '/', 'c'}; // 含冒号
    static constexpr OCTET rootless2[] = {'!', '$', '&', '/', 'd'}; // sub-delims
    static_assert(path(make_span2(rootless1)));
    static_assert(path(make_span2(rootless2)));

    // ------------------------- 无效路径测试 -------------------------
    // 无效的path-absolute (只有单个"/")
    static constexpr OCTET invalid_absolute[] = {'/'};
    static_assert(
        path_absolute(make_span2(invalid_absolute))); // path-absolute需要segment-nz
    static_assert(path_abempty(make_span2(invalid_absolute))); // 但属于path-abempty

    // 无效的path-noscheme (起始segment含冒号)
    static constexpr OCTET invalid_noscheme[] = {'a', ':', 'b', '/', 'c'};
    static_assert(!path_noscheme(make_span2(invalid_noscheme))); // 起始segment不能有冒号
    static_assert(path_rootless(make_span2(invalid_noscheme)));  // 但属于path-rootless

    // 无效字符 (空格)
    static constexpr OCTET invalid_space[] = {'a', ' ', '/', 'b'};
    static_assert(!path(make_span2(invalid_space)));

    // 不完整百分比编码
    static constexpr OCTET invalid_pct1[] = {'%', 'A'};
    static constexpr OCTET invalid_pct2[] = {'a', '%', '2'};
    static_assert(!path(make_span2(invalid_pct1)));
    static_assert(!path(make_span2(invalid_pct2)));

    // ------------------------- 边界测试 -------------------------
    // 最大长度segment (255字节)
    static constexpr OCTET max_segment[] = {
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', // 60
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 80
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 100
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 120
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 140
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 160
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 180
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 200
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a', // 220
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
        'a', 'a', 'a', 'a', 'a',                                                  // 240
        'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a' // 255
    };
    static_assert(path_rootless(make_span2(max_segment))); // 255字节有效

    // 混合类型压力测试
    static constexpr OCTET complex_path[] = {'/', 'a', ':', 'b', '/', '%', '4',
                                             '1', '/', '@', '!', '/', 'c', 'd',
                                             ':', 'e', '/', 'f', 'g', 'h'};
    static_assert(path(make_span2(complex_path))); // 属于path-abempty

}; // namespace mcs::abnf::uri::test

#include <iostream>

int main()
{
    using namespace mcs::abnf::uri::test;
    {
        { // 常规网站路径
            static constexpr OCTET website_path[] = {'/', 'u', 's', 'e', 'r', 's',
                                                     '/', '1', '2', '3', '/', 'p',
                                                     'r', 'o', 'f', 'i', 'l', 'e'};
            static_assert(path(make_span2(website_path)));
        }

        { // 多级RESTful API路径
            static constexpr OCTET rest_api_path[] = {
                '/', 'a', 'p', 'i', '/', 'v', '2', '/', 'o', 'r', 'd', 'e', 'r',
                's', '/', '2', '0', '2', '3', '-', '1', '0', '-', '1', '5'};
            static_assert(path(make_span2(rest_api_path)));
        }

        { // 包含编码字符的文件路径
            static constexpr OCTET encoded_file[] = {
                '/', 'd', 'o', 'c', 's', '/', 'm', 'y', '%', '2', '0', '%', 'C', 'A',
                'F', 'E', '%', 'E', '6', 'B', '5', '8', 'B', '.', 'p', 'd', 'f'};
            static_assert(path(make_span2(encoded_file)));
        }

        { // 带参数的动态路径
            static constexpr OCTET dynamic_path[] = {
                '/', 'p', 'r', 'o', 'd', 'u', 'c', 't', 's', '/', 'c',
                'a', 't', ':', 'e', 'l', 'e', 'c', 't', 'r', 'o', 'n',
                'i', 'c', 's', '/', 'i', 'd', ':', '1', '2', '3'};
            static_assert(path(make_span2(dynamic_path)));
        }

        { // 电子商务分类路径
            static constexpr OCTET ecommerce_path[] = {
                '/', 'c', 'a', 't', 'e', 'g', 'o', 'r', 'y', '/', '2', '5', '%', 'o',
                'f', 'f', '/', 's', 'u', 'm', 'm', 'e', 'r', '-', 's', 'a', 'l', 'e'};
            static_assert(not path(make_span2(ecommerce_path)));
        }

        { // 社交媒体路径
            static constexpr OCTET social_media[] = {
                '/', '@', 'u', 's', 'e', 'r', 'n', 'a', 'm', 'e',
                '/', 'p', 'o', 's', 't', 's', '/', '1', '2', '3',
                '4', '5', '?', 'k', 'e', 'y', '=', 'v', 'a', 'l'};
            static_assert(not path(make_span2(social_media))); // query部分需要单独测试
        }
        {
            { // 带合法百分比编码的路径
                static constexpr OCTET valid_pct_path[] = {
                    '/', 'e', '%', '2', '0', 'x', '%', '2', '6', 'm',
                    'p', 'l', 'e', '/', '%', 'C', '3', '%', 'A', '9'}; // /e x&mple/é
                static_assert(path(make_span2(valid_pct_path)));
            }

            { // 包含冒号的合法路径（path-rootless）
                static constexpr OCTET colon_path[] = {'a', 'p', 'p', ':', 'v', '1',
                                                       '.', '0', '/', 's', 'e', 't',
                                                       't', 'i', 'n', 'g', 's'};
                static_assert(path(make_span2(colon_path)));
            }

            { // 带查询参数的完整URI解析
                static constexpr OCTET full_uri[] = {'/', 'p', 'a', 't', 'h', '?', 'q',
                                                     '=', '1', '#', 'f', 'r', 'a', 'g'};
                static_assert(path(make_span2(full_uri).first(5))); // 只验证/path部分
                static_assert(query(make_span2(full_uri).subspan(6, 1))); // 验证query部分

                static_assert(!pchar('#')); // ❌ 属于gen-delims
                static_assert(
                    not fragment(make_span2(full_uri).subspan(8))); // 验证fragment部分
            }
            {
                { // 有效片段测试
                    static constexpr OCTET valid_frag[] = {'f', 'r', 'a', 'g', '?',
                                                           '/', '%', '2', '0'};
                    static_assert(fragment(make_span2(valid_frag)));
                }

                { // 无效片段测试（包含#）
                    static constexpr OCTET invalid_frag[] = {'f', 'r', 'a', 'g', '#'};
                    static_assert(!fragment(make_span2(invalid_frag)));
                }
            }
            {
                { // 带查询参数的完整URI解析
                    static constexpr OCTET full_uri[] = {
                        '/', 'p', 'a', 't', 'h', // Path: /path (indices 0-4)
                        '?', 'q', '=', '1',      // Query: q=1 (indices 5-8)
                        '#', 'f', 'r', 'a', 'g'  // Fragment: frag (indices 10-13)
                    };
                    // NOTE: '#' 要编码才能使用
                    static_assert(not path(make_span2(full_uri)));
                    {
                        { // 合法片段：包含编码后的# (%23)
                            static constexpr OCTET encoded_hash[] = {'f', 'r', '%', '2',
                                                                     '3', 'a', 'g'};
                            static_assert(fragment(make_span2(encoded_hash)).has_value());
                        }

                        { // 非法片段：直接包含#
                            static constexpr OCTET raw_hash[] = {'f', 'r', '#', 'a', 'g'};
                            static_assert(!fragment(make_span2(raw_hash)).has_value());
                        }

                        { // 合法查询：包含?和/
                            static constexpr OCTET valid_query[] = {'?', 'q', '=', '/',
                                                                    '1', '?', '2'};
                            static_assert(query(make_span2(valid_query)).has_value());
                        }
                    }

                    // 验证路径部分 (indices 0-4)
                    static_assert(path(make_span2(full_uri).first(5)));

                    // 验证查询部分 (indices 5-8)
                    static_assert(query(make_span2(full_uri).subspan(5, 4)));

                    // 验证片段部分 (indices 9-13)
                    static_assert(fragment(make_span2(full_uri).subspan(10)));
                }
            }
        }

        { // 国际化路径 (UTF-8编码后) // 不支持
          // static constexpr OCTET i18n_path[] = {
          //     '/', 'n', 'a', 'm', 'e',  '/', '%',  'E',
          //     '4', '%', 'B', '8', '%',  'A', 'D',       // 中
          //     '%', 'E', '6', '%', '96', '%', '87',      // 文
          //     '%', 'E', '8', '%', 'B',  '7', '%',  'AF' // 路
          // };
          // static_assert(not path(make_span2(i18n_path)));
        }

        { // 复杂混合路径
            static constexpr OCTET complex_mixed[] = {
                '/', 'd', 'a', 't', 'a', '(', 't', 'y', 'p', 'e', '=', 'r',
                'e', 'p', 'o', 'r', 't', ')', '/', '2', '0', '2', '3', '/',
                'v', 'e', 'r', 's', 'i', 'o', 'n', '%', '3', 'A', '1', '.',
                '0', '/', '@', 'd', 'e', 't', 'a', 'i', 'l', 's'};
            static_assert(path(make_span2(complex_mixed)));
        }

        { // 超长但有效路径 // 不支持，目前仅仅看 网络的字符
          // static constexpr OCTET
          //     long_valid_path[] =
          //         {
          //             '/', 'a', 'b', 'c', 'd', 'e',  'f', 'g', 'h', 'i', 'j', 'k',
          //             'l', 'm', 'n', 'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w',
          //             'x', 'y', 'z', '/', '0', '1',  '2', '3', '4', '5', '6', '7',
          //             '8', '9', '0', '1', '2', '3',  '4', '5', '6', '7', '8', '9',
          //             '/', '!', '@', '$', '&', '(',  ')', '*', '+', ',', ';', '=',
          //             '/', '%', 'E', '3', '%', '81', '%', '82' // 日文字符あ的编码
          //         };
          // static_assert(not path(make_span2(long_valid_path)));
        }

        { // 带版本号的API路径
            static constexpr OCTET versioned_api[] = {
                '/', 'v', '3', '.', '1', '4', '/', 'u', 's', 'e', 'r', 's', '/',
                'a', 'c', 't', 'i', 'o', 'n', ':', 'u', 'p', 'd', 'a', 't', 'e'};
            static_assert(path(make_span2(versioned_api)));
        }
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND