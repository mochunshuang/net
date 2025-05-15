#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     *  regular       = "art-lojban"        ; these tags match the 'langtag'
                        / "cel-gaulish"       ; production, but their subtags
                        / "no-bok"            ; are not extended language
                        / "no-nyn"            ; or variant subtags: their meaning
                        / "zh-guoyu"          ; is defined by their registration
                        / "zh-hakka"          ; and all of these are deprecated
                        / "zh-min"            ; in favor of a more modern
                        / "zh-min-nan"        ; subtag or sequence of subtags
                        / "zh-xiang"
     *
     */
    constexpr auto regular_pass = make_pass_test<regular>();
    constexpr auto regular_fail = make_unpass_test<regular>();
    static_assert(regular_pass("art-lojban"_span));
    static_assert(regular_pass("aRt-lojban"_span));
    static_assert(regular_pass("aRt-LOJBAN"_span));

    static_assert(regular_pass("cel-gaulish"_span));
    static_assert(regular_pass("no-bok"_span));
    static_assert(regular_pass("no-nyn"_span));

    static_assert(regular_pass("zh-guoyu"_span));
    static_assert(regular_pass("zh-hakka"_span));
    static_assert(regular_pass("zh-guoyu"_span));
    static_assert(regular_pass("zh-min"_span));
    static_assert(regular_pass("zh-min-nan"_span));
    static_assert(regular_pass("zh-xiang"_span));

    static_assert(!regular_fail("L2n"_span));
    static_assert(!regular_fail("Latin"_span));

    return 0;
}
// NOLINTEND