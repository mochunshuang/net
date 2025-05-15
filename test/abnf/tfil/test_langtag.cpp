#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     *  extlang       = 3ALPHA              ; selected ISO 639 codes
     *                  *2("-" 3ALPHA)      ; permanently reserved
     *
     */
    /**
       * language   = 2*3ALPHA            ; shortest ISO 639 code
                      ["-" extlang]       ; sometimes followed by
                                       ; extended language subtags
                      / 4ALPHA              ; or reserved for future use
                      / 5*8ALPHA            ; or registered language subtag
       *
       */
    //  script        = 4ALPHA
    /**
     * region        = 2ALPHA              ; ISO 3166-1 code
                     / 3DIGIT              ; UN M.49 code
     *
     */
    // variant       = 5*8alphanum
    // extension     = singleton 1*("-" (2*8alphanum))
    // privateuse    = "x" 1*("-" (1*8alphanum))
    /**
      *  langtag       = language
                  ["-" script]
                  ["-" region]
                  *("-" variant)
                  *("-" extension)
                  ["-" privateuse]
      *
      */
    constexpr auto langtag_pass = make_pass_test<langtag>();
    constexpr auto langtag_fail = make_unpass_test<langtag>();
    static_assert(langtag_pass("zh-Hans"_span));
    assert(langtag_pass("zh-Hans"_span)); // NOTE: 和 extlang 前缀一致冲突
    {
        static_assert(make_pass_test<language>()("zh"_span));
        static_assert(make_pass_test<script>()("Hans"_span));

        // language
        static_assert(make_pass_test<langtag_rule>()("zh"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-yue"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc"_span));

        // language + script
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans"_span));

        // language + script + region
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-CN"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-001"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-CN"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-001"_span));

        static_assert(make_pass_test<langtag_rule>()("zh-001"_span));

        // language + script + region + variant
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-CN-0abc"_span));
        static_assert(
            make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-CN-abcnd"_span));
        static_assert(
            make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-CN-abcdefgh"_span));
        static_assert(
            make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-abcdefgh"_span));
        // language  + variant
        static_assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-abcdefgh"_span));
        // assert(make_pass_test<langtag_rule>()("zh-yue-fra-abc-abcdefgh"_span));

        // language + script + region + variant + extension
        static_assert(
            make_pass_test<langtag_rule>()("zh-yue-fra-abc-Hans-CN-0abc-a-ext"_span));
        static_assert(make_pass_test<langtag_rule>()(
            "zh-yue-fra-abc-Hans-CN-0abc-a-ext-b-abcdefg-0-abc"_span));
        static_assert(make_pass_test<langtag_rule>()("zh-a-ext-b-abcdefg-0-abc"_span));
    }
    static_assert(langtag_pass("zh-Hans-CN-a-ext-x-private"_span)); // 完整结构
    {
        static_assert(make_pass_test<language>()("zh"_span));
        static_assert(make_pass_test<script>()("Hans"_span));
        static_assert(make_pass_test<region>()("CN"_span));
        static_assert(make_pass_test<extension>()("a-ext"_span));
        static_assert(make_pass_test<privateuse>()("x-private"_span));
    }
    static_assert(langtag_pass("en-Latn-US-valencia"_span)); // 多组件
    static_assert(langtag_fail("zh--CN"_span));              // 空组件

    static_assert(
        langtag_pass("en-Latn-123"_span)); // 数字地区错误,region 不对具体数字要求

    return 0;
}
// NOLINTEND