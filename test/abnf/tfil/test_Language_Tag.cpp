#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     * Language-Tag  = langtag             ; normal language tags
                   / privateuse          ; private use tag
                   / grandfathered       ; grandfathered tags
     *
     */
    constexpr auto language_tag_pass_test = make_pass_test<Language_Tag>();
    constexpr auto language_tag_fail_test = make_unpass_test<Language_Tag>();

    // 基础语言标签测试
    static_assert(language_tag_pass_test("zh-CN"_span));           // 合法
    static_assert(language_tag_pass_test("en-Latn-US"_span));      // 合法
    static_assert(language_tag_pass_test("es-419"_span));          // 合法
    static_assert(language_tag_pass_test("de-CH-x-phonebk"_span)); // 合法

    static_assert(language_tag_fail_test("en--US"_span)); // 非法：双连字符
    static_assert(language_tag_fail_test("zh_CN"_span));  // 非法：错误分隔符
    static_assert(language_tag_fail_test("x1-abc"_span)); // 非法：私有标签格式错误

    return 0;
}
// NOLINTEND