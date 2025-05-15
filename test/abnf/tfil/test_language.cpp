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
    constexpr auto language_pass = make_pass_test<language>();
    constexpr auto language_fail = make_unpass_test<language>();

    //-------------------------
    // 2*3ALPHA + 可选扩展语言
    //-------------------------
    // 合法案例
    static_assert(language_pass("zh"_span));             // 2字母基础代码
    static_assert(language_pass("eng"_span));            // 3字母基础代码
    static_assert(language_pass("zh-yue"_span));         // 带扩展语言
    static_assert(language_pass("ast-ara"_span));        // 3字母+扩展
    static_assert(language_pass("zh-yue-fra"_span));     // 多重扩展（应为extlang处理）
    static_assert(language_pass("zh-yue-fra-abc"_span)); // 多重扩展（应为extlang处理）

    // 非法案例
    static_assert(!language_fail("a"_span));      // 过短 (1字母)
    static_assert(language_fail("zh--yue"_span)); // 双连字符
    static_assert(language_fail("eng123"_span));  // 包含数字

    static_assert(language_fail("zh-yue-fra-abc-def"_span)); // 4 个 extlang 是错误的

    //-------------------------
    // 4ALPHA（保留代码）
    //-------------------------
    static_assert(language_pass("qaaa"_span));  // 保留4字母代码
    static_assert(language_pass("qabx"_span));  // 合法保留格式
    static_assert(language_pass("qaa"_span));   // 过短
    static_assert(language_pass("qaaab"_span)); // 过长

    static_assert(language_fail("qa2a"_span)); // 包含数字

    //-------------------------
    // 5*8ALPHA（注册代码）
    //-------------------------
    static_assert(language_pass("asdfg"_span));     // 5字母注册代码
    static_assert(language_pass("abcdefgh"_span));  // 8字母注册代码
    static_assert(language_pass("tokipona"_span));  // 有效注册示例
    static_assert(language_pass("abcd"_span));      // 过短 (4字母)
    static_assert(language_fail("abcdefghi"_span)); // 过长 (9字母)
    static_assert(!language_fail("c++"_span));      // 非法字符

    //-------------------------
    // 边界混合测试
    //-------------------------
    static_assert(language_pass("zh-yue-fra"_span)); // 最多 3个 extlang
    static_assert(language_pass("aaaa"_span));       // 4字母应匹配保留分支
    static_assert(language_pass("aaa"_span));        // 3字母必须匹配基础分支
    static_assert(language_pass("aaaaa"_span));      // 5字母应匹配注册分支

    return 0;
}
// NOLINTEND