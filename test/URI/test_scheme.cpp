

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::ABNF;
    using namespace mcs::ABNF::URI; // NOLINT

    //================= scheme 有效用例 =================
    // 基本合法格式
    constexpr auto basic_scheme = "http"_span; // 纯小写字母
    constexpr auto mixed_case_scheme =
        "FTP"_span;                          // 大写开头混合大小写（规范允许但推荐小写）
    constexpr auto with_numbers = "s3"_span; // 字母+数字
    constexpr auto with_special_chars = "my+app-1.0"_span; // 包含所有允许符号（+ - .）
    constexpr auto single_char = "x"_span;                 // 最小长度
    constexpr auto max_length = "a1234567890.+-.zzz"_span; // 长合法组合

    static_assert(scheme(basic_scheme));
    static_assert(scheme(mixed_case_scheme));
    static_assert(scheme(with_numbers));
    static_assert(scheme(with_special_chars));
    static_assert(scheme(single_char));
    static_assert(scheme(max_length));

    //================= scheme 无效用例 =================
    // 非法首字符
    constexpr auto starts_with_number = "3d-printer"_span; // 首字符非ALPHA
    constexpr auto starts_with_dot = ".hidden"_span;       // 首字符为点号
    constexpr auto starts_with_plus = "+stream"_span;      // 首字符为+

    // 包含非法字符
    constexpr auto has_underscore = "my_scheme"_span; // 下划线非法
    constexpr auto has_space = "git hub"_span;        // 包含空格
    constexpr auto has_percent =
        "auth%20"_span; // 含百分号（应使用pct-encoded在其他部分）

    // 结构错误
    constexpr auto empty_string = ""_span;          // 空字符串
    constexpr auto only_special = "+-."_span;       // 无起始字母
    constexpr auto trailing_colon = "scheme:"_span; // 包含URI分隔符（:）

    static_assert(!scheme(starts_with_number));
    static_assert(!scheme(starts_with_dot));
    static_assert(!scheme(starts_with_plus));
    static_assert(!scheme(has_underscore));
    static_assert(!scheme(has_space));
    static_assert(!scheme(has_percent));
    static_assert(!scheme(empty_string));
    static_assert(!scheme(only_special));
    static_assert(!scheme(trailing_colon));

    // 极端用例：看似合法但含隐藏错误
    constexpr auto unicode_char = "utf✓8"_span;    // 含Unicode字符（✓不属于ALPHA）
    constexpr auto control_char = "ht\x01tp"_span; // 含控制字符
    static_assert(!scheme(unicode_char));
    static_assert(!scheme(control_char));

    return 0;
}

// NOLINTEND