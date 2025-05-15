#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{ /**
      * Language-Tag  = langtag             ; normal language tags
                    / privateuse          ; private use tag
                    / grandfathered       ; grandfathered tags
      *
      */
    // Content-Language = [ language-tag *( OWS "," OWS language-tag ) ]
    constexpr auto content_lang_pass =
        make_pass_test<mcs::abnf::http::content_Language>();
    constexpr auto content_lang_fail =
        make_unpass_test<mcs::abnf::http::content_Language>();

    static_assert(content_lang_pass("zh-CN"_span));        // 带地区代码
    static_assert(content_lang_pass("en, fr  , de"_span)); // 多语言
    {
        // langtag
        static_assert(make_pass_test<tfil::langtag>()("zh-CN"_span));
    }

    static_assert(content_lang_fail("en_US"_span)); // 非法下划线

    return 0;
}
// NOLINTEND