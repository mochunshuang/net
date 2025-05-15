#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     * irregular    = "en-GB-oed"         ; irregular tags do not match
                    / "i-ami"             ; the 'langtag' production and
                    / "i-bnn"             ; would not otherwise be
                    / "i-default"         ; considered 'well-formed'
                    / "i-enochian"        ; These tags are all valid,
                    / "i-hak"             ; but most are deprecated
                    / "i-klingon"         ; in favor of more modern
                    / "i-lux"             ; subtags or subtag
                    / "i-mingo"           ; combination
                    / "i-navajo"
                    / "i-pwn"
                    / "i-tao"
                    / "i-tay"
                    / "i-tsu"
                    / "sgn-BE-FR"
                    / "sgn-BE-NL"
                    / "sgn-CH-DE"
     *
     */
    constexpr auto irregular_pass = make_pass_test<irregular>();
    constexpr auto irregular_fail = make_unpass_test<irregular>();

    static_assert(irregular_pass("en-GB-oed"_span));
    static_assert(irregular_pass("En-GB-oed"_span));
    static_assert(irregular_pass("eN-GB-oed"_span));
    static_assert(irregular_pass("en-gB-oed"_span));
    static_assert(irregular_pass("en-gB-oeD"_span));

    static_assert(irregular_pass("i-ami"_span));
    static_assert(irregular_pass("i-bnn"_span));
    static_assert(irregular_pass("i-default"_span));
    static_assert(irregular_pass("i-enochian"_span));
    static_assert(irregular_pass("i-hak"_span));
    static_assert(irregular_pass("i-klingon"_span));
    static_assert(irregular_pass("i-lux"_span));
    static_assert(irregular_pass("i-mingo"_span));
    static_assert(irregular_pass("i-navajo"_span));
    static_assert(irregular_pass("i-pwn"_span));
    static_assert(irregular_pass("i-tao"_span));
    static_assert(irregular_pass("i-tay"_span));
    static_assert(irregular_pass("i-tsu"_span));

    static_assert(irregular_pass("sgn-BE-FR"_span));
    static_assert(irregular_pass("sgn-BE-NL"_span));
    static_assert(irregular_pass("sgn-CH-DE"_span));

    static_assert(irregular_fail("en-GB-oed2"_span));
    static_assert(!irregular_fail("abc"_span));
    static_assert(!irregular_fail("13578"_span));

    return 0;
}
// NOLINTEND